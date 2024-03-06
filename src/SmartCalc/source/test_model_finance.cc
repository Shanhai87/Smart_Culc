#include <gtest/gtest.h>

#include "../include/model_finance.h"

class test_model_finance : public testing::Test {
 protected:
  s21::CreditInfo credit_info;
  s21::CreditResponse credit_responce;
  s21::DepositInfo deposit_info;
  s21::DepositResponse deposit_responce;
  s21::ModelFinance *model_finance;

  void SetUp() { model_finance = new s21::ModelFinance(); }
  void TearDown() { delete model_finance; }
};

TEST_F(test_model_finance, test_1) {
  credit_info.credit_amount = 1000000;
  credit_info.credit_period = 2;
  credit_info.period_type = s21::kYears;
  credit_info.interest_rate = 10;
  credit_info.payment_type = s21::kAnnuity;
  credit_responce = model_finance->BeginCredit(credit_info);
  ASSERT_EQ(credit_responce.payment[0], "46144.93");
  ASSERT_EQ(credit_responce.overpayment, "107478.32");
  ASSERT_EQ(credit_responce.total_payment, "1107478.32");
}

TEST_F(test_model_finance, test_2) {
  credit_info.credit_amount = 1000000;
  credit_info.credit_period = 3;
  credit_info.period_type = s21::kMonths;
  credit_info.interest_rate = 10;
  credit_info.payment_type = s21::kDifferentiated;
  credit_responce = model_finance->BeginCredit(credit_info);
  ASSERT_EQ(credit_responce.payment[0], "341666.67");
  ASSERT_EQ(credit_responce.payment[1], "338888.89");
  ASSERT_EQ(credit_responce.payment[2], "336111.11");
  ASSERT_EQ(credit_responce.overpayment, "16666.67");
  ASSERT_EQ(credit_responce.total_payment, "1016666.67");
}

TEST_F(test_model_finance, test_3) {
  deposit_info.deposit_amount = 1000000;
  deposit_info.deposit_period = 100;
  deposit_info.period_type = s21::kDays;
  deposit_info.interest_rate = 10;
  deposit_info.tax_rate = 0;
  deposit_info.deposit_type = s21::kMonthly;
  deposit_info.deposit_date = "30.06.2023";
  deposit_info.capitalization = false;
  deposit_responce = model_finance->BeginDeposit(deposit_info);
  ASSERT_EQ(deposit_responce.interest_amount, "27397.26");
  ASSERT_EQ(deposit_responce.tax_amount, "0");
  ASSERT_EQ(deposit_responce.total_amount, "1000000");
}

TEST_F(test_model_finance, test_4) {
  deposit_info.deposit_amount = 100000;
  deposit_info.deposit_period = 2;
  deposit_info.period_type = s21::kYears;
  deposit_info.interest_rate = 5;
  deposit_info.tax_rate = 0;
  deposit_info.deposit_type = s21::kYearly;
  deposit_info.deposit_date = "30.06.2023";
  deposit_info.capitalization = true;
  deposit_info.partial_deposits = {"15.10.2023-5000", "28.02.2024-3000"};
  deposit_info.partial_withdrawals = {"7.10.2024-10000"};
  deposit_responce = model_finance->BeginDeposit(deposit_info);
  ASSERT_EQ(deposit_responce.interest_amount, "10524.2");
  ASSERT_EQ(deposit_responce.tax_amount, "0");
  ASSERT_EQ(deposit_responce.total_amount, "108524.2");
}
