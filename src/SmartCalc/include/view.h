#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <set>
#include <vector>

#include "qcustomplot.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace s21 {

class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QWidget *parent = nullptr);
  ~View();
  double GetGraphXMin();
  double GetGraphXMax();
  AngleUnit GetAngleUnit();
  double GetGraphCountPoins();
  char GetGraphVar();
  std::map<char, std::string> GetVariables();
  std::string GetScreenText();
  void Update(Info info);
  void DrawGraph(std::vector<double> vector_x, std::vector<double> vector_y);
  bool IsGraphMode();
  bool IsFinMode();

  void SetPayment(std::vector<std::string> payment);
  void SetOverpayment(std::string overpayment);
  void SetTotalAmount(std::string total_amount);
  void SetInterestFromDeposit(std::string interest);
  void SetTaxAmount(std::string interest);
  void SetDepositTotalAmount(std::string total_amount);

 signals:
  void enter_clicked();
  void change_mode();
  void change_angle_unit(AngleUnit arg);
  void change_graph_var(char arg);
  void change_graph_x_min(double arg);
  void change_graph_x_max(double arg);
  void change_number_points(int arg);
  void calculate_credit(CreditInfo arg);
  void calculate_deposit(DepositInfo arg);

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private slots:
  void on_button_enter_clicked();
  void on_button_graph_clicked();
  void on_button_fin_clicked();
  void on_button_message_clicked();
  void on_message_textChanged();
  void on_screen_textChanged();
  void on_button_rad_clicked();
  void on_button_deg_clicked();
  void keyboardButtonPressed();
  void operationButtonPressed();
  void setMaxLength();
  void on_comboBox_currentTextChanged();
  void on_x_min_editingFinished();
  void on_x_max_editingFinished();
  void on_y_min_editingFinished();
  void on_y_max_editingFinished();
  void on_x_min_valueChanged(double arg);
  void on_x_max_valueChanged(double arg);
  void on_y_min_valueChanged(double arg);
  void on_y_max_valueChanged(double arg);
  void on_count_points_valueChanged(int arg);
  void on_button_credit_clicked();
  void on_button_deposit_clicked();

 private:
  Ui::MainWindow *ui_;
  AngleUnit angle_unit_;
  std::array<QPushButton *, 21> button_array_;
  std::array<Qt::Key, 21> key_array_;

  void SetInitialView();
  void MakeVisibleVariables(std::set<char> set, char variable,
                            QTextEdit *widget);
  void EmitKey(Qt::Key key);

  void ConnectButtonSignalsToSlots();
};
}  // namespace s21

#endif  // MAINWINDOW_H
