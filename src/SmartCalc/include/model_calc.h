#ifndef S21_MODEL_CALC_H
#define S21_MODEL_CALC_H

#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "utils.h"

namespace s21 {

class ModelCalc {
 public:
  ModelCalc();

  std::vector<double> GetGraphX();
  std::vector<double> GetGraphY();
  void SetGraphVar(char axis);
  void SetXMin(double value);
  void SetXMax(double value);
  void SetVisibleGraph(bool visible_graph);
  void SetVisibleFin(bool visible_fin);
  void SetNumberPoints(int count);
  void SetVariables(std::map<char, std::string> variables);
  void SetAngleUnit(AngleUnit unit);
  Info Begin(std::string str);

 private:
  class Lexeme {
   public:
    enum LexemeType {
      kLOperation,
      kROperation,
      kCloseBracket,
      kOpenBracket,
      kOperand,
      kError,
      kVariable,
      kFunction
    };

    Lexeme();
    Lexeme(std::string str, LexemeType type = kError, int priority = 0);

    std::string data_;
    LexemeType type_;
    int priority_;
  };

  void Start(std::string &str, char var = '\0');
  void ClearData(std::string &src, char var);
  void ClearSubString(std::string &src_string, std::string del_string);
  void MakeLexemes(std::string &str, std::vector<Lexeme> *lexeme, char var);
  char *LexNumber(std::vector<Lexeme> *lexeme, char *ptr, bool &operation,
                  bool &operand);
  char *GetNumber(std::vector<Lexeme> *lexeme, char *ptr);
  char *DotCase(std::string &str, bool &dot, char *ptr);
  char *GetExponent(std::vector<Lexeme> *lexeme, char *ptr);
  char *LexOperation(std::vector<Lexeme> *lexeme, char *ptr, bool &operation,
                     bool &operand);
  char *LexBracket(std::vector<Lexeme> *lexeme, char *ptr, bool &operation,
                   bool &operand);
  char *LexFunction(std::vector<Lexeme> *lexeme, char *ptr, bool &operation,
                    bool &operand);
  int CheckFunction(std::vector<Lexeme> *lexeme, char *ptr, const char *str,
                    bool &operation, bool &operand);
  char *LexVariable(char *ptr, bool &operation, bool &operand);
  char *LexSpace(char *ptr);
  void AddMultiplication(std::vector<Lexeme> *lexeme);
  void ValidateString(std::vector<Lexeme> *lexeme);
  void SetError(Lexeme *lexeme);
  void SetErrorStr(std::string &str);
  void CheckLexemeOrder(Lexeme *lexeme, Lexeme::LexemeType type_b);
  void CheckBrackets(std::vector<Lexeme> *lexeme);
  void CheckCountBrackets(Lexeme *lexeme, Lexeme::LexemeType type_a,
                          int &count);
  void MakeResponse(std::string &str, std::vector<Lexeme> *lexeme, char var);
  std::queue<Lexeme> ApplyDijkstraAlgorithm(std::vector<Lexeme> *lexeme);
  void CaseRightOperation(std::queue<Lexeme> &queue, std::stack<Lexeme> &stack,
                          Lexeme &tmp);
  void CaseLeftOperation(std::queue<Lexeme> &queue, std::stack<Lexeme> &stack,
                         Lexeme &tmp);
  void CaseCloseBracket(std::queue<Lexeme> &queue, std::stack<Lexeme> &stack);
  void PutToQueueFromStack(std::queue<Lexeme> &queue,
                           std::stack<Lexeme> &stack);
  double CalculateAnswer(std::queue<Lexeme> rp_notation);
  void MakeOperationExpression(std::stack<double> &stack, std::string operation,
                               double *result);
  void MakeFunctionExpression(std::stack<double> &stack, std::string function,
                              double *result);
  void GetVariableValueForExpression(std::stack<double> &stack,
                                     std::string variable);
  double StringToDouble(const std::string &str, int precision);
  std::string DoubleToString(const double number, int precision);
  void CalculateOperation(std::string operation, double first, double second,
                          double *result);
  void CalculateFunction(std::string operation, double first, double *result);
  void SaveResult(double &result, char var);
  void GetGraphData(std::queue<Lexeme> rp_notation);

 private:
  std::vector<Lexeme> lexemes_;
  std::vector<Lexeme> lexemes_var_;
  std::vector<const char *> functions_;
  Info response_;
  int error_var_;
  std::map<char, double> var_result_;
  AngleUnit angle_unit_;
  bool visible_graph_;
  bool visible_fin_;
  std::vector<double> graph_x_;
  std::vector<double> graph_y_;
  char graph_var_;
  double x_min_;
  double x_max_;
  int number_points_;
};
}  // namespace s21

#endif  // S21_MODEL_GRAPH_H
