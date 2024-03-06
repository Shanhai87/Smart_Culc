#ifndef S21_CONTROLLER_H
#define S21_CONTROLLER_H

#include <QWidget>

#include "model.h"
#include "view.h"

namespace s21 {

class Controller : public QWidget {
  Q_OBJECT

 public:
  Controller(View *view, Model *model);
  ~Controller();

 private slots:
  void on_enter_clicked();
  void on_change_mode();
  void on_change_angle_unit(AngleUnit arg);
  void on_change_graph_var(char arg);
  void on_change_graph_x_min(double arg);
  void on_change_graph_x_max(double arg);
  void on_change_number_points(int arg);
  void on_calculate_credit(CreditInfo arg);
  void on_calculate_deposit(DepositInfo arg);

 private:
  View *view_;
  Model *model_;

  void SetInitialModel();
  void ConnectViewSignalsToSlots();
  Info RedrawGraph();
};

}  // namespace s21

#endif  // S21_CONTROLLER_H
