#include "../include/controller.h"

s21::Controller::Controller(View *view, Model *model)
    : view_(view), model_(model) {
  SetInitialModel();
  ConnectViewSignalsToSlots();
}

s21::Controller::~Controller() {}

void s21::Controller::SetInitialModel() {
  model_->SetXMin(view_->GetGraphXMin());
  model_->SetXMax(view_->GetGraphXMax());
  model_->SetNumberPoints(view_->GetGraphCountPoins());
  model_->SetAngleUnit(view_->GetAngleUnit());
  model_->SetGraphVar(view_->GetGraphVar());
}

void s21::Controller::ConnectViewSignalsToSlots() {
  connect(view_, SIGNAL(enter_clicked()), this, SLOT(on_enter_clicked()));
  connect(view_, SIGNAL(change_mode()), this, SLOT(on_change_mode()));
  connect(view_, SIGNAL(change_angle_unit(AngleUnit)), this,
          SLOT(on_change_angle_unit(AngleUnit)));
  connect(view_, SIGNAL(change_graph_var(char)), this,
          SLOT(on_change_graph_var(char)));
  connect(view_, SIGNAL(change_graph_x_min(double)), this,
          SLOT(on_change_graph_x_min(double)));
  connect(view_, SIGNAL(change_graph_x_max(double)), this,
          SLOT(on_change_graph_x_max(double)));
  connect(view_, SIGNAL(change_number_points(int)), this,
          SLOT(on_change_number_points(int)));
  connect(view_, SIGNAL(calculate_credit(CreditInfo)), this,
          SLOT(on_calculate_credit(CreditInfo)));
  connect(view_, SIGNAL(calculate_deposit(DepositInfo)), this,
          SLOT(on_calculate_deposit(DepositInfo)));
}

s21::Info s21::Controller::RedrawGraph() {
  Info info;
  model_->SetVariables(view_->GetVariables());
  info = model_->Begin(view_->GetScreenText());
  if (view_->IsGraphMode())
    view_->DrawGraph(model_->GetGraphX(), model_->GetGraphY());
  return info;
}

void s21::Controller::on_enter_clicked() { view_->Update(RedrawGraph()); }

void s21::Controller::on_change_mode() {
  model_->SetVisibleGraph(view_->IsGraphMode());
  model_->SetVisibleFin(view_->IsFinMode());
}

void s21::Controller::on_change_angle_unit(AngleUnit arg) {
  model_->SetAngleUnit(arg);
}

void s21::Controller::on_change_graph_var(char arg) {
  model_->SetGraphVar(arg);
}

void s21::Controller::on_change_graph_x_min(double arg) {
  model_->SetXMin(arg);
  RedrawGraph();
}

void s21::Controller::on_change_graph_x_max(double arg) {
  model_->SetXMax(arg);
  RedrawGraph();
}

void s21::Controller::on_change_number_points(int arg) {
  model_->SetNumberPoints(arg);
  RedrawGraph();
}

void s21::Controller::on_calculate_credit(CreditInfo arg) {
  CreditResponse tmp = model_->BeginCredit(arg);
  view_->SetPayment(tmp.payment);
  view_->SetOverpayment(tmp.overpayment);
  view_->SetTotalAmount(tmp.total_payment);
}

void s21::Controller::on_calculate_deposit(DepositInfo arg) {
  DepositResponse response = model_->BeginDeposit(arg);
  view_->SetInterestFromDeposit(response.interest_amount);
  view_->SetTaxAmount(response.tax_amount);
  view_->SetDepositTotalAmount(response.total_amount);
}
