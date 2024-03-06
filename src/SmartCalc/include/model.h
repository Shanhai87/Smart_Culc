#ifndef S21_MODEL_H
#define S21_MODEL_H

#include "model_calc.h"
#include "model_finance.h"

namespace s21 {

class Model : public ModelCalc, public ModelFinance {
 public:
  Model(){};
};
}  // namespace s21

#endif  // S21_MODEL_H
