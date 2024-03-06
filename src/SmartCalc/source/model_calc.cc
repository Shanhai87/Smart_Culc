#include "../include/model_calc.h"

s21::ModelCalc::ModelCalc()
    : functions_{"cos",  "sin",  "tan", "acos", "asin",
                 "atan", "sqrt", "ln",  "lg"},
      angle_unit_(AngleUnit::kDegree),
      visible_graph_(false),
      graph_var_('\0'),
      x_min_(-20),
      x_max_(20),
      number_points_(100) {}

std::vector<double> s21::ModelCalc::GetGraphX() { return graph_x_; }

std::vector<double> s21::ModelCalc::GetGraphY() { return graph_y_; }

void s21::ModelCalc::SetGraphVar(char axis) { graph_var_ = axis; }

void s21::ModelCalc::SetXMin(double value) { x_min_ = value; }

void s21::ModelCalc::SetXMax(double value) { x_max_ = value; }

void s21::ModelCalc::SetVisibleGraph(bool visible_graph) {
  visible_graph_ = visible_graph;
}

void s21::ModelCalc::SetVisibleFin(bool visible_fin) {
  visible_fin_ = visible_fin;
}

void s21::ModelCalc::SetNumberPoints(int count) { number_points_ = count; }

void s21::ModelCalc::SetVariables(std::map<char, std::string> variables) {
  response_.variables = variables;
}

void s21::ModelCalc::SetAngleUnit(AngleUnit unit) { angle_unit_ = unit; }

s21::Info s21::ModelCalc::Begin(std::string str) {
  Start(str);
  return response_;
}

void s21::ModelCalc::Start(std::string &str, char var) {
  ClearData(str, var);
  if (!str.empty()) {
    std::vector<Lexeme> *lexeme;
    if (!var)
      lexeme = &lexemes_;
    else
      lexeme = &lexemes_var_;
    MakeLexemes(str, lexeme, var);
    if (!lexeme->empty()) {
      AddMultiplication(lexeme);
      ValidateString(lexeme);
      MakeResponse(str, lexeme, var);
      if ((var && !error_var_) || (!var && !response_.error)) {
        std::queue<Lexeme> rp_notation = ApplyDijkstraAlgorithm(lexeme);
        double result = CalculateAnswer(rp_notation);
        SaveResult(result, var);
        if (!var && visible_graph_) GetGraphData(rp_notation);
      }
    }
  }
}

void s21::ModelCalc::ClearData(std::string &src, char var) {
  ClearSubString(src, "<font color = red>");
  ClearSubString(src, "</font>");
  if (!var) {
    graph_x_.clear();
    graph_y_.clear();
    lexemes_.clear();
    response_.message.clear();
    response_.answer = src;
    response_.error = 0;
    var_result_.clear();
  } else {
    lexemes_var_.clear();
  }
  error_var_ = 0;
}

void s21::ModelCalc::ClearSubString(std::string &src_str, std::string del_str) {
  size_t pos = std::string::npos;
  while ((pos = src_str.find(del_str)) != std::string::npos)
    src_str.erase(pos, del_str.length());
}

void s21::ModelCalc::MakeLexemes(std::string &str, std::vector<Lexeme> *lexeme,
                                 char var) {
  bool operation_before = false;
  bool operand_before = false;
  char *prev = nullptr;
  for (char *tmp = str.data(); *tmp;) {
    prev = tmp;
    tmp = LexNumber(lexeme, tmp, operation_before, operand_before);
    tmp = LexOperation(lexeme, tmp, operation_before, operand_before);
    tmp = LexBracket(lexeme, tmp, operation_before, operand_before);
    tmp = LexFunction(lexeme, tmp, operation_before, operand_before);
    if (!var) tmp = LexVariable(tmp, operation_before, operand_before);
    tmp = LexSpace(tmp);
    if (prev == tmp)
      lexeme->push_back(Lexeme(std::string(tmp++, 1), Lexeme::kError));
  }
}

char *s21::ModelCalc::LexNumber(std::vector<Lexeme> *lexeme, char *ptr,
                                bool &operation, bool &operand) {
  if ((*ptr > 47 && *ptr < 58) || *ptr == '.') {
    ptr = GetNumber(lexeme, ptr);
    operation = false;
    operand = true;
  }
  return ptr;
}

char *s21::ModelCalc::GetNumber(std::vector<Lexeme> *lexeme, char *ptr) {
  std::string number;
  bool dot = false;
  if (*ptr == '.') ptr = DotCase(number, dot, ptr);
  for (; *ptr > 47 && *ptr < 58; ++ptr) number.append(std::string{*ptr});
  if (*ptr == '.' && !dot) ptr = DotCase(number, dot, ptr);
  for (; *ptr > 47 && *ptr < 58; ++ptr) number.append(std::string{*ptr});
  if (number.size() > 1 && number.at(0) == '.') number.insert(0, "0");
  if (number.size() > 1 && *(--number.end()) == '.')
    number.erase(--number.end());
  lexeme->push_back(Lexeme(number, Lexeme::kOperand, 1));
  if (*ptr == 'e') ptr = GetExponent(lexeme, ++ptr);
  return ptr;
}

char *s21::ModelCalc::DotCase(std::string &str, bool &dot, char *ptr) {
  dot = true;
  str.append(".");
  return ++ptr;
}

char *s21::ModelCalc::GetExponent(std::vector<Lexeme> *lexeme, char *ptr) {
  std::string number;
  lexeme->push_back(Lexeme("*", Lexeme::kLOperation, 2));
  lexeme->push_back(Lexeme("10", Lexeme::kOperand, 1));
  lexeme->push_back(Lexeme("^", Lexeme::kROperation, 3));
  lexeme->push_back(Lexeme("(", Lexeme::kOpenBracket));
  if (*ptr == '+' || *ptr == '-') {
    lexeme->push_back(Lexeme("0", Lexeme::kOperand, 1));
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kLOperation, 1));
  }
  if (*ptr < 48 || *ptr > 57)
    lexeme->push_back(Lexeme("0", Lexeme::kOperand, 1));
  else {
    for (; *ptr > 47 && *ptr < 58; ++ptr) number.append(std::string{*ptr});
    lexeme->push_back(Lexeme(number, Lexeme::kOperand, 1));
  }
  lexeme->push_back(Lexeme(")", Lexeme::kCloseBracket));
  return ptr;
}

char *s21::ModelCalc::LexOperation(std::vector<Lexeme> *lexeme, char *ptr,
                                   bool &operation, bool &operand) {
  if ((*ptr == '+' || *ptr == '-') && operation == operand) {
    if (!operation) lexeme->push_back(Lexeme("0", Lexeme::kOperand, 1));
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kLOperation, 1));
    operation = true;
  } else if (*ptr == '+' || *ptr == '-') {
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kLOperation, 1));
    operation = true;
  } else if (*ptr == '*' || *ptr == '/') {
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kLOperation, 2));
    operation = true;
  } else if (*ptr == '^') {
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kROperation, 3));
    operation = true;
  } else if (*ptr == 'm' && *(ptr + 1) == 'o' && *(ptr + 2) == 'd') {
    lexeme->push_back(Lexeme(std::string(ptr, 3), Lexeme::kLOperation, 2));
    operation = true;
    ptr += 3;
  }
  return ptr;
}

char *s21::ModelCalc::LexBracket(std::vector<Lexeme> *lexeme, char *ptr,
                                 bool &operation, bool &operand) {
  if (*ptr == '(') {
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kOpenBracket));
    operand = false;
    operation = false;
  } else if (*ptr == ')') {
    lexeme->push_back(Lexeme(std::string{*ptr++}, Lexeme::kCloseBracket));
    operand = true;
    operation = false;
  }
  return ptr;
}

char *s21::ModelCalc::LexFunction(std::vector<Lexeme> *lexeme, char *ptr,
                                  bool &operation, bool &operand) {
  int result = 0;
  auto iter = functions_.begin();
  for (; iter != functions_.end() && !result; ++iter)
    result = CheckFunction(lexeme, ptr, *iter, operation, operand);
  return ptr + result;
}

int s21::ModelCalc::CheckFunction(std::vector<Lexeme> *lexeme, char *ptr,
                                  const char *str, bool &operation,
                                  bool &operand) {
  int result = 0;
  for (; *str && *ptr == *str; ++result, ++ptr, ++str) {
  }
  if (*str)
    result = 0;
  else {
    lexeme->push_back(
        Lexeme(std::string(ptr - result, result), Lexeme::kFunction, 4));
    operand = false;
    operation = true;
  }
  return result;
}

char *s21::ModelCalc::LexVariable(char *ptr, bool &operation, bool &operand) {
  if (*ptr > 119 && *ptr < 123) {
    if (!response_.variables[*ptr].empty())
      Start(response_.variables[*ptr], *ptr);
    lexemes_.push_back(Lexeme(std::string{*ptr++}, Lexeme::kVariable, 1));
    operation = false;
    operand = true;
  }
  return ptr;
}

char *s21::ModelCalc::LexSpace(char *ptr) {
  if ((*ptr > 8 && *ptr < 14) || *ptr == ' ') ++ptr;
  return ptr;
}

void s21::ModelCalc::AddMultiplication(std::vector<Lexeme> *lexeme) {
  Lexeme::LexemeType type;
  Lexeme::LexemeType type_before = (*lexeme->begin()).type_;
  for (size_t i = 1; i < lexeme->size(); ++i) {
    type = (*(lexeme->begin() + i)).type_;
    if ((type == Lexeme::kOpenBracket || type == Lexeme::kFunction ||
         type == Lexeme::kVariable) &&
        (type_before == Lexeme::kOperand || type_before == Lexeme::kVariable ||
         type_before == Lexeme::kCloseBracket)) {
      lexeme->insert(lexeme->begin() + i++,
                     Lexeme("*", Lexeme::kLOperation, 2));
    } else if (type == Lexeme::kOperand &&
               (type_before == Lexeme::kVariable ||
                type_before == Lexeme::kCloseBracket))
      lexeme->insert(lexeme->begin() + i++,
                     Lexeme("*", Lexeme::kLOperation, 2));
    type_before = type;
  }
}

void s21::ModelCalc::ValidateString(std::vector<Lexeme> *lexeme) {
  std::vector<Lexeme>::iterator iter = lexeme->begin();
  Lexeme *tmp = &*iter;
  Lexeme::LexemeType type = tmp->type_;
  Lexeme::LexemeType type_before = type;
  if (type == Lexeme::kLOperation || type == Lexeme::kROperation ||
      type == Lexeme::kCloseBracket || type == Lexeme::kError ||
      tmp->data_ == ".")
    SetError(tmp);
  for (++iter; iter != lexeme->end(); ++iter) {
    tmp = &*iter;
    CheckLexemeOrder(tmp, type_before);
    if (tmp->type_ != Lexeme::kError) type_before = tmp->type_;
  }
  type = tmp->type_;
  if (type == Lexeme::kLOperation || type == Lexeme::kROperation ||
      type == Lexeme::kOpenBracket || type == Lexeme::kFunction)
    SetError(tmp);
  CheckBrackets(lexeme);
}

void s21::ModelCalc::SetError(Lexeme *lexeme) {
  SetErrorStr(lexeme->data_);
  error_var_ = 1;
  response_.error = 1;
}

void s21::ModelCalc::SetErrorStr(std::string &str) {
  str.insert(0, "<font color = red>");
  str.insert(str.size(), "</font>");
}

void s21::ModelCalc::CheckLexemeOrder(Lexeme *lexeme,
                                      Lexeme::LexemeType type_b) {
  Lexeme::LexemeType type_a = lexeme->type_;
  if ((type_a == Lexeme::kLOperation || type_a == Lexeme::kROperation ||
       type_a == Lexeme::kCloseBracket) &&
      (type_b == Lexeme::kLOperation || type_b == Lexeme::kROperation ||
       type_b == Lexeme::kOpenBracket || type_b == Lexeme::kFunction))
    SetError(lexeme);
  else if (type_a == Lexeme::kError)
    SetError(lexeme);
  else if ((type_a == Lexeme::kOperand) &&
           (lexeme->data_ == "." || type_b == Lexeme::kOperand))
    SetError(lexeme);
  else if (type_a == Lexeme::kFunction && type_b == Lexeme::kFunction)
    SetError(lexeme);
}

void s21::ModelCalc::CheckBrackets(std::vector<Lexeme> *lexeme) {
  std::vector<Lexeme>::iterator iter = lexeme->begin();
  Lexeme *tmp;
  int count = 0;
  for (; iter != lexeme->end(); ++iter) {
    tmp = &*iter;
    CheckCountBrackets(tmp, Lexeme::kCloseBracket, count);
  }
  count = 0;
  for (size_t i = lexeme->size(); i > 0; --i) {
    tmp = &*(--iter);
    CheckCountBrackets(tmp, Lexeme::kOpenBracket, count);
  }
}

void s21::ModelCalc::CheckCountBrackets(Lexeme *lexeme,
                                        Lexeme::LexemeType type_a, int &count) {
  Lexeme::LexemeType type_b;
  if (type_a == Lexeme::kCloseBracket)
    type_b = Lexeme::kOpenBracket;
  else
    type_b = Lexeme::kCloseBracket;
  if (lexeme->type_ == type_a && !count)
    SetError(lexeme);
  else if (lexeme->type_ == type_a)
    --count;
  else if (lexeme->type_ == type_b)
    ++count;
}

void s21::ModelCalc::MakeResponse(std::string &str, std::vector<Lexeme> *lexeme,
                                  char var) {
  Lexeme tmp;
  size_t i = 0;
  if ((var && error_var_) || (!var && response_.error)) {
    str.clear();
    while (i < lexeme->size()) {
      tmp = lexeme->at(i++);
      str.append(tmp.data_);
      str.append(" ");
    }
  }
  if (!var)
    response_.message = str;
  else
    response_.variables[var] = str;
}

std::queue<s21::ModelCalc::Lexeme> s21::ModelCalc::ApplyDijkstraAlgorithm(
    std::vector<Lexeme> *lexeme) {
  std::queue<Lexeme> output;
  std::stack<Lexeme> stack;
  auto iter = lexeme->begin();
  Lexeme tmp;
  for (; iter != lexeme->end(); ++iter) {
    tmp = *iter;
    if (tmp.type_ == Lexeme::kOperand || tmp.type_ == Lexeme::kVariable)
      output.push(tmp);
    else if (tmp.type_ == Lexeme::kFunction ||
             tmp.type_ == Lexeme::kOpenBracket)
      stack.push(tmp);
    else if (tmp.type_ == Lexeme::kROperation)
      CaseRightOperation(output, stack, tmp);
    else if (tmp.type_ == Lexeme::kLOperation)
      CaseLeftOperation(output, stack, tmp);
    else if (tmp.type_ == Lexeme::kCloseBracket)
      CaseCloseBracket(output, stack);
  }
  while (!stack.empty()) PutToQueueFromStack(output, stack);
  return output;
}

void s21::ModelCalc::CaseRightOperation(std::queue<Lexeme> &queue,
                                        std::stack<Lexeme> &stack,
                                        Lexeme &tmp) {
  while (!stack.empty() && ((stack.top().type_ == Lexeme::kROperation &&
                             tmp.priority_ < stack.top().priority_) ||
                            stack.top().type_ == Lexeme::kFunction))
    PutToQueueFromStack(queue, stack);
  stack.push(tmp);
}

void s21::ModelCalc::CaseLeftOperation(std::queue<Lexeme> &queue,
                                       std::stack<Lexeme> &stack, Lexeme &tmp) {
  while (!stack.empty() && ((stack.top().type_ == Lexeme::kLOperation &&
                             tmp.priority_ <= stack.top().priority_) ||
                            tmp.priority_ < stack.top().priority_))
    PutToQueueFromStack(queue, stack);
  stack.push(tmp);
}

void s21::ModelCalc::CaseCloseBracket(std::queue<Lexeme> &queue,
                                      std::stack<Lexeme> &stack) {
  while (!stack.empty() && stack.top().type_ != Lexeme::kOpenBracket)
    PutToQueueFromStack(queue, stack);
  stack.pop();
  if (!stack.empty() && stack.top().type_ == Lexeme::kFunction)
    PutToQueueFromStack(queue, stack);
}

void s21::ModelCalc::PutToQueueFromStack(std::queue<Lexeme> &queue,
                                         std::stack<Lexeme> &stack) {
  queue.push(stack.top());
  stack.pop();
}

double s21::ModelCalc::CalculateAnswer(std::queue<Lexeme> rp_notation) {
  std::stack<double> stack;
  Lexeme tmp;
  double result;
  while (!rp_notation.empty() && !error_var_) {
    tmp = rp_notation.front();
    if (tmp.type_ == Lexeme::kOperand)
      stack.push(StringToDouble(tmp.data_, 8));
    else if (tmp.type_ == Lexeme::kLOperation ||
             tmp.type_ == Lexeme::kROperation)
      MakeOperationExpression(stack, tmp.data_, &result);
    else if (tmp.type_ == Lexeme::kFunction)
      MakeFunctionExpression(stack, tmp.data_, &result);
    else
      GetVariableValueForExpression(stack, tmp.data_);
    rp_notation.pop();
  }
  if (error_var_)
    result = 0;
  else
    result = stack.top();
  return result;
}

void s21::ModelCalc::MakeOperationExpression(std::stack<double> &stack,
                                             std::string operation,
                                             double *result) {
  double second = stack.top();
  stack.pop();
  CalculateOperation(operation, stack.top(), second, result);
  stack.pop();
  stack.push(*result);
}

void s21::ModelCalc::MakeFunctionExpression(std::stack<double> &stack,
                                            std::string function,
                                            double *result) {
  CalculateFunction(function, stack.top(), result);
  stack.pop();
  stack.push(*result);
}

void s21::ModelCalc::GetVariableValueForExpression(std::stack<double> &stack,
                                                   std::string variable) {
  if (!var_result_.count(variable[0]))
    response_.error = error_var_ = 1;
  else
    stack.push(var_result_[variable[0]]);
}

double s21::ModelCalc::StringToDouble(const std::string &str, int precision) {
  double x;
  std::istringstream istream(str);
  istream.precision(precision);
  if (!(istream >> x)) return NAN;
  return x;
};

std::string s21::ModelCalc::DoubleToString(const double number, int precision) {
  std::ostringstream ostream;
  ostream.precision(precision);
  ostream << number;
  return ostream.str();
};

void s21::ModelCalc::CalculateOperation(std::string operation, double first,
                                        double second, double *result) {
  if (operation == "+")
    *result = first + second;
  else if (operation == "-")
    *result = first - second;
  else if (operation == "*")
    *result = first * second;
  else if (operation == "/") {
    if (!second)
      *result = NAN;
    else
      *result = first / second;
  } else if (operation == "mod") {
    double int_part1, int_part2;
    if (fabs(second) < 1)
      *result = NAN;
    else if (!modf(first, &int_part1) && !modf(second, &int_part2))
      *result = (int)int_part1 % (int)int_part2;
    else
      *result = NAN;
  } else
    *result = pow(first, second);
}

void s21::ModelCalc::CalculateFunction(std::string function, double first,
                                       double *result) {
  if (angle_unit_ == kDegree &&
      (function == "sin" || function == "cos" || function == "tan"))
    first *= M_PI / 180;
  if (function == "cos")
    *result = cos(first);
  else if (function == "sin")
    *result = sin(first);
  else if (function == "tan")
    *result = tan(first);
  else if (function == "acos")
    *result = acos(first);
  else if (function == "asin")
    *result = asin(first);
  else if (function == "atan")
    *result = atan(first);
  else if (function == "sqrt")
    *result = sqrt(first);
  else if (function == "ln")
    *result = log(first);
  else if (function == "lg")
    *result = log10(first);
  if (fabs(*result) < 1e-8) *result = 0;
  if (angle_unit_ == kDegree &&
      (function == "asin" || function == "acos" || function == "atan"))
    *result *= 180 / M_PI;
}

void s21::ModelCalc::SaveResult(double &result, char var) {
  if (var) {
    var_result_.emplace(var, result);
    if (result != result) SetErrorStr(response_.variables[var]);
  } else {
    if (result == INFINITY)
      response_.answer = "Infinity";
    else if (result == -INFINITY)
      response_.answer = "-Infinity";
    else if (result != result) {
      response_.answer = "ERROR";
      SetErrorStr(response_.message);
    } else
      response_.answer = DoubleToString(result, 8);
  }
}

void s21::ModelCalc::GetGraphData(std::queue<Lexeme> rp_notation) {
  error_var_ = 0;
  for (int i = 0; i < number_points_; ++i) {
    var_result_[graph_var_] =
        x_min_ + i * (x_max_ - x_min_) / (number_points_ - 1);
    graph_x_.push_back(x_min_ + i * (x_max_ - x_min_) / (number_points_ - 1));
    graph_y_.push_back(CalculateAnswer(rp_notation));
    if (error_var_) {
      graph_x_.clear();
      graph_y_.clear();
      return;
    }
  }
}

s21::ModelCalc::Lexeme::Lexeme() : data_(), type_(kOperand), priority_() {}

s21::ModelCalc::Lexeme::Lexeme(std::string str, LexemeType type, int priority)
    : data_(str), type_(type), priority_(priority) {}
