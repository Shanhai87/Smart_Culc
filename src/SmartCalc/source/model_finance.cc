#include "../include/model_finance.h"

#include <cmath>
#include <iomanip>
#include <sstream>

s21::ModelFinance::ModelFinance()
    : payment_type_(kAnnuity), period_type_(kMonths) {}

s21::CreditResponse s21::ModelFinance::BeginCredit(CreditInfo credit_info) {
  CreditResponse response;
  if (credit_info.credit_amount && credit_info.credit_period &&
      credit_info.interest_rate) {
    SaveCreditInfo(credit_info);
    Start(response);
  } else {
    response.payment.push_back("0");
    response.overpayment = response.total_payment = "0";
  }
  return response;
}

s21::DepositResponse s21::ModelFinance::BeginDeposit(DepositInfo deposit_info) {
  SaveDepositInfo(deposit_info);
  DepositResponse response;
  all_partial_amounts_.clear();
  error_ = 0;
  if (deposit_amount_ && deposit_period_ && deposit_interest_rate_ &&
      !deposit_date_.empty()) {
    std::vector<Deposit> dates;
    IsValidStringDate(deposit_date_, dates, 0);
    if (!error_) IsValidPartialAmounts(partial_deposits_, dates, 1);
    if (!error_) IsValidPartialAmounts(partial_withdrawals_, dates, -1);
    if (!error_) GetLastDate(dates);
    for (size_t i = 1; i < dates.size(); ++i)
      all_partial_amounts_.emplace(
          ConvertDateToCountDays(dates[i].year, dates[i].month, dates[i].day),
          dates[i].amount);
    AddInterest(dates);
    StartDeposit(response);
  } else
    error_ = 1;
  if (error_) MakeErrorResponse(response);
  return response;
}

void s21::ModelFinance::SaveCreditInfo(CreditInfo credit_info) {
  credit_amount_ = credit_info.credit_amount;
  credit_period_ = credit_info.credit_period;
  interest_rate_ = credit_info.interest_rate;
  payment_type_ = credit_info.payment_type;
  period_type_ = credit_info.period_type;
}

void s21::ModelFinance::Start(CreditResponse &response) {
  CreditPeriodToMonths();
  if (payment_type_ == kAnnuity)
    CalculateAnnuityCredit(response);
  else
    CalculateDifferetiatedCredit(response);
}

void s21::ModelFinance::CreditPeriodToMonths() {
  if (period_type_ == kDays)
    DaysToMonths();
  else if (period_type_ == kYears)
    credit_period_ *= 12;
}

void s21::ModelFinance::DaysToMonths() {
  int result = 1;
  for (int i = 45; i < credit_period_;) {
    result += 1;
    i += 31;
    if (i < credit_period_) result += 1;
    i += 30;
  }
  credit_period_ = result;
}

void s21::ModelFinance::CalculateAnnuityCredit(CreditResponse &response) {
  double monthly_rate = interest_rate_ / 12 / 100;
  double payment = credit_amount_ * monthly_rate *
                   pow((1 + monthly_rate), credit_period_) /
                   (pow(1 + monthly_rate, credit_period_) - 1);
  payment = round(payment * 100) / 100;
  response.payment.push_back(DoubleToStringNoZero(payment));
  double total_payment = payment * credit_period_;
  SavePayments(response, total_payment);
}

void s21::ModelFinance::CalculateDifferetiatedCredit(CreditResponse &response) {
  double payment;
  double total_payment = 0;
  double payment1 = credit_amount_ * 1.0 / credit_period_;
  double monthly_rate = interest_rate_ / 12 / 100;
  for (int i = 1; i <= credit_period_; ++i) {
    payment = payment1 + (credit_amount_ - payment1 * (i - 1)) * monthly_rate;
    payment = round(payment * 100) / 100;
    response.payment.push_back(DoubleToStringNoZero(payment));
    total_payment += payment;
  }
  SavePayments(response, total_payment);
}

void s21::ModelFinance::SavePayments(CreditResponse &response,
                                     double total_payment) {
  response.total_payment = DoubleToStringNoZero(total_payment);
  double overpayment = total_payment - credit_amount_;
  response.overpayment = DoubleToStringNoZero(overpayment);
}

std::string s21::ModelFinance::DoubleToStringNoZero(double value) {
  std::ostringstream oss;
  oss << std::setprecision(12) << std::noshowpoint << value;
  return oss.str();
}

void s21::ModelFinance::SaveDepositInfo(DepositInfo deposit_info) {
  deposit_amount_ = deposit_info.deposit_amount;
  deposit_date_ = deposit_info.deposit_date;
  deposit_period_ = deposit_info.deposit_period;
  deposit_period_type_ = deposit_info.period_type;
  deposit_interest_rate_ = deposit_info.interest_rate;
  tax_rate_ = deposit_info.tax_rate;
  capitalization_ = deposit_info.capitalization;
  deposit_type_ = deposit_info.deposit_type;
  partial_deposits_ = deposit_info.partial_deposits;
  partial_withdrawals_ = deposit_info.partial_withdrawals;
}

void s21::ModelFinance::IsValidStringDate(std::string str,
                                          std::vector<Deposit> &dates,
                                          int sign) {
  Deposit tmp{0, 0, 0, 0};
  int size = str.length();
  char *ptr = str.data();
  for (int i = 0; i < size; ++i) {
    if (!tmp.day)
      tmp.day = GetIntValueFromString(ptr, i, size);
    else if (!tmp.month)
      tmp.month = GetIntValueFromString(ptr, i, size);
    else if (!tmp.year)
      tmp.year = GetIntValueFromString(ptr, i, size);
    else if (!tmp.amount && sign)
      tmp.amount = GetIntValueFromString(ptr, i, size);
  }
  if (sign)
    tmp.amount *= sign;
  else
    tmp.amount = deposit_amount_;
  if (!tmp.day || !tmp.month || !tmp.year || !tmp.amount) error_ = 1;
  CheckDateInfo(tmp);
  if (!error_) dates.push_back(tmp);
}

int s21::ModelFinance::GetIntValueFromString(char *ptr, int &i, int size) {
  int value = 0;
  while (ptr[i] >= '0' && ptr[i] <= '9' && i < size)
    value = 10 * value + ptr[i++] - 48;
  return value;
}

void s21::ModelFinance::CheckDateInfo(Deposit &date) {
  if (date.month > 12 || date.month < 1)
    error_ = 1;
  else if (date.day > (30 + (date.month + (date.month / 8)) % 2) ||
           date.day < 1)
    error_ = 1;
  else if (date.month == 2 && IsLeapYear(date.year) && date.day > 29)
    error_ = 1;
  else if (date.month == 2 && !IsLeapYear(date.year) && date.day > 28)
    error_ = 1;
  else if (date.year <= 0 || date.year > 9999)
    error_ = 1;
}

bool s21::ModelFinance::IsLeapYear(int year) {
  bool result = false;
  if (year % 400 == 0)
    result = true;
  else if (year % 100 == 0)
    result = false;
  else if (year % 4 == 0)
    result = true;
  return result;
}

void s21::ModelFinance::IsValidPartialAmounts(std::vector<std::string> part,
                                              std::vector<Deposit> &dates,
                                              int sign) {
  if (!error_ && part.size()) {
    for (auto it = part.begin(); it != part.end() && !error_; ++it)
      IsValidStringDate(*it, dates, sign);
  }
}

void s21::ModelFinance::GetLastDate(std::vector<Deposit> &dates) {
  Deposit tmp{0, 0, 0, 0};
  day_min_ =
      ConvertDateToCountDays(dates[0].year, dates[0].month, dates[0].day);
  if (deposit_period_type_ == kDays) {
    day_max_ = day_min_ + deposit_period_;
  } else if (deposit_period_type_ == kMonths) {
    tmp.year = dates[0].year + (dates[0].month + deposit_period_ - 1) / 12;
    tmp.month = (dates[0].month + deposit_period_ - 1) % 12 + 1;
    tmp.day = dates[0].day;
    CorrectDayInDate(tmp);
    day_max_ = ConvertDateToCountDays(tmp.year, tmp.month, tmp.day);
  } else {
    tmp = dates[0];
    tmp.year += deposit_period_;
    CorrectDayInDate(tmp);
    day_max_ = ConvertDateToCountDays(tmp.year, tmp.month, tmp.day);
  }
}

int s21::ModelFinance::ConvertDateToCountDays(int year, int month, int day) {
  if (month < 3) {
    --year;
    month += 12;
  }
  return 365 * year + year / 4 - year / 100 + year / 400 +
         (153 * month - 457) / 5 + day - 306;
}

void s21::ModelFinance::CorrectDayInDate(Deposit &date) {
  do {
    error_ = 0;
    CheckDateInfo(date);
  } while (error_ && --date.day);
}

void s21::ModelFinance::AddInterest(const std::vector<Deposit> &dates) {
  if (deposit_type_ == kDaily)
    for (int i = day_min_; i <= day_max_; ++i)
      all_partial_amounts_.emplace(i, -1);
  else if (deposit_type_ == kMonthly)
    PutDepositInMap(dates, 1);
  else if (deposit_type_ == kQuarterly)
    PutDepositInMap(dates, 3);
  else
    PutDepositInMap(dates, 12);
}

void s21::ModelFinance::PutDepositInMap(const std::vector<Deposit> &dates,
                                        int k) {
  Deposit tmp{0, 0, 0, 0};
  int days = 0;
  for (int i = 1; days < day_max_; ++i) {
    SetDateInMonths(dates, tmp, k * i);
    CorrectDayInDate(tmp);
    days = ConvertDateToCountDays(tmp.year, tmp.month, tmp.day);
    if (days > day_max_) days = day_max_;
    all_partial_amounts_.emplace(days, -1);
  }
}

void s21::ModelFinance::SetDateInMonths(const std::vector<Deposit> &dates,
                                        Deposit &date, int offset) {
  date.year = dates[0].year + (dates[0].month + offset - 1) / 12;
  date.month = (dates[0].month + offset - 1) % 12 + 1;
  date.day = dates[0].day;
}

void s21::ModelFinance::StartDeposit(DepositResponse &response) {
  int tmp = day_min_;
  double delta = 0;
  double interest = 0;
  for (auto it = all_partial_amounts_.begin(); it != all_partial_amounts_.end();
       ++it) {
    CheckLeapDifference(tmp, (*it).first, delta);
    CalculateDepositInterest(tmp, (*it).first, delta);
    if ((*it).second == -1) {
      interest += delta;
      if (capitalization_) deposit_amount_ += delta;
      delta = 0;
    } else
      deposit_amount_ += (*it).second;
    tmp = (*it).first;
  }
  MakeResponse(response, interest);
}

void s21::ModelFinance::CheckLeapDifference(int &first, const int &second,
                                            double &delta) {
  if (IsLeapDay(first) != IsLeapDay(second)) {
    int day = second;
    int tmp_day;
    for (int i = 1; day > 0; ++i, day -= 365) {
      tmp_day = day;
      if (!(i % 400) || (!(i % 4) && i % 100)) day -= 1;
    }
    tmp_day = second - tmp_day;
    CalculateDepositInterest(first, tmp_day, delta);
    first = tmp_day;
  }
}

bool s21::ModelFinance::IsLeapDay(int day) {
  int i = 0;
  for (; day > 0; ++i, day -= 365)
    if (!(i % 400) || (!(i % 4) && i % 100)) day -= 1;
  return IsLeapYear(i);
}

void s21::ModelFinance::CalculateDepositInterest(const int &first,
                                                 const int &second,
                                                 double &delta) {
  double delta_tmp;
  int leap_number;
  if (IsLeapDay(second))
    leap_number = 366;
  else
    leap_number = 365;
  delta_tmp = deposit_amount_ * deposit_interest_rate_ * (second - first) /
              100.0 / leap_number;
  delta_tmp = round(delta_tmp * 100) / 100.0;
  delta += delta_tmp;
}

void s21::ModelFinance::MakeResponse(DepositResponse &response,
                                     const double &interest) {
  double tax = interest * tax_rate_ / 100.0;
  tax = round(tax * 100) / 100.0;
  response.interest_amount = DoubleToStringNoZero(interest);
  response.tax_amount = DoubleToStringNoZero(tax);
  response.total_amount = DoubleToStringNoZero(deposit_amount_);
}

void s21::ModelFinance::MakeErrorResponse(DepositResponse &response) {
  response.interest_amount = "0";
  response.tax_amount = "0";
  response.total_amount = "0";
}
