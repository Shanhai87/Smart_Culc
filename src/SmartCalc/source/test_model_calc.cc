#include <gtest/gtest.h>

#include "../include/model_calc.h"

class test_model_calc : public testing::Test {
 protected:
  s21::ModelCalc *model_calc;
  std::string input;
  s21::Info response;

  void SetUp() { model_calc = new s21::ModelCalc(); }
  void TearDown() { delete model_calc; }
};

TEST_F(test_model_calc, test_1) {
  input = "2+2*2";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "6");
}

TEST_F(test_model_calc, test_2) {
  input = "(5/7+3/14)/(20/7)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "0.325");
}

TEST_F(test_model_calc, test_3) {
  input = "((7-6.35)/6.5+9.9)/((1.2/36+1.2/0.25-21/16)/(169/24))";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "20");
}

TEST_F(test_model_calc, test_4) {
  input =
      "((2+3/4)/1.1+3+1/3)/(2.5-0.4*(3+1/3))/(5/7)-(2+1/6+4.5)*0.375/"
      "(2.75-1.5)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "5");
}

TEST_F(test_model_calc, test_5) {
  input = "2sin30^2+cos30^2";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "1.25");
}

TEST_F(test_model_calc, test_6) {
  input = "24/(sin127^2+1+sin217^2)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "12");
}

TEST_F(test_model_calc, test_7) {
  model_calc->SetAngleUnit(s21::kRadian);
  input = "sqrt48-sqrt192sin(19*3.14159265/12)^2";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "-6");
}

TEST_F(test_model_calc, test_8) {
  input = "lg289/lg17(ln(1/500)/ln500)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "-2");
}

TEST_F(test_model_calc, test_9) {
  input = "16^(lg5/lg2)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "625");
}

TEST_F(test_model_calc, test_10) {
  input = "1/0";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "ERROR");
}

TEST_F(test_model_calc, test_11) {
  input = "1e-3";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "0.001");
}

TEST_F(test_model_calc, test_12) {
  input = "2^(11mod8)^2";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "512");
}

TEST_F(test_model_calc, test_13) {
  input = "(tan22+tan23)/(1-tan22tan23)";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "1");
}

TEST_F(test_model_calc, test_14) {
  input = "atan1";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "45");
}

TEST_F(test_model_calc, test_15) {
  input = "asin1";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "90");
}

TEST_F(test_model_calc, test_16) {
  input = "acos1";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "0");
}

TEST_F(test_model_calc, test_17) {
  input = "acos2";
  response = model_calc->Begin(input);
  ASSERT_EQ(response.answer, "ERROR");
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
