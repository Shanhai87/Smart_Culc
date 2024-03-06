#ifndef S21_UTILS_H
#define S21_UTILS_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace s21 {

enum AngleUnit { kRadian, kDegree };

enum PaymentType { kAnnuity, kDifferentiated };

enum PeriodType { kDays, kMonths, kYears };

enum PaymentsFrequency { kDaily, kMonthly, kQuarterly, kYearly };

struct Info {
  std::string message;
  std::string answer;
  std::map<char, std::string> variables;
  int error;
};

struct CreditInfo {
  double credit_amount;
  int credit_period;
  double interest_rate;
  PaymentType payment_type;
  PeriodType period_type;
};

struct CreditResponse {
  std::vector<std::string> payment;
  std::string overpayment;
  std::string total_payment;
};

struct DepositInfo {
  double deposit_amount;
  std::string deposit_date;
  int deposit_period;
  PeriodType period_type;
  double interest_rate;
  double tax_rate;
  bool capitalization;
  PaymentsFrequency deposit_type;
  std::vector<std::string> partial_deposits;
  std::vector<std::string> partial_withdrawals;
};

struct DepositResponse {
  std::string interest_amount;
  std::string tax_amount;
  std::string total_amount;
};

struct Deposit {
  int day;
  int month;
  int year;
  int amount;
};

}  // namespace s21

#endif  // S21_UTILS_H
