#ifndef S21_MODEL_FINANCE_H
#define S21_MODEL_FINANCE_H

#include "utils.h"

namespace s21 {
class ModelFinance {
 public:
  ModelFinance();

  CreditResponse BeginCredit(CreditInfo credit_info);
  DepositResponse BeginDeposit(DepositInfo credit_info);

 private:
  double credit_amount_;
  int credit_period_;
  double interest_rate_;
  PaymentType payment_type_;
  PeriodType period_type_;

  double deposit_amount_;
  std::string deposit_date_;
  int deposit_period_;
  PeriodType deposit_period_type_;
  double deposit_interest_rate_;
  double tax_rate_;
  bool capitalization_;
  PaymentsFrequency deposit_type_;
  std::vector<std::string> partial_deposits_;
  std::vector<std::string> partial_withdrawals_;
  int error_;
  int day_min_;
  int day_max_;
  std::multimap<int, int> all_partial_amounts_;

  void SaveCreditInfo(CreditInfo credit_info);
  void Start(CreditResponse &response);
  void CreditPeriodToMonths();
  void DaysToMonths();
  void CalculateAnnuityCredit(CreditResponse &response);
  void CalculateDifferetiatedCredit(CreditResponse &response);
  void SavePayments(CreditResponse &response, double total_payment);
  std::string DoubleToStringNoZero(double value);

  void SaveDepositInfo(DepositInfo deposit_info);
  void IsValidStringDate(std::string str, std::vector<Deposit> &dates,
                         int sign);
  int GetIntValueFromString(char *ptr, int &i, int size);
  void CheckDateInfo(Deposit &date);
  bool IsLeapYear(int year);
  void IsValidPartialAmounts(std::vector<std::string> part,
                             std::vector<Deposit> &dates, int sign);
  void GetLastDate(std::vector<Deposit> &dates);
  int ConvertDateToCountDays(int y, int m, int d);
  void CorrectDayInDate(Deposit &date);
  void AddInterest(const std::vector<Deposit> &dates);
  void PutDepositInMap(const std::vector<Deposit> &dates, int k);
  void SetDateInMonths(const std::vector<Deposit> &dates, Deposit &date,
                       int offset);
  void StartDeposit(DepositResponse &response);
  void CheckLeapDifference(int &first, const int &second, double &delta);
  bool IsLeapDay(int day);
  void CalculateDepositInterest(const int &first, const int &second,
                                double &delta);
  void MakeResponse(DepositResponse &response, const double &interest);
  void MakeErrorResponse(DepositResponse &response);
};
}  // namespace s21

#endif  // S21_MODEL_FINANCE_H
