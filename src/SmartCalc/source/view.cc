#include "../include/view.h"

#include "../ui_mainwindow.h"

s21::View::View(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      angle_unit_(AngleUnit::kDegree),
      key_array_{Qt::Key_0,          Qt::Key_1,         Qt::Key_2,
                 Qt::Key_3,          Qt::Key_4,         Qt::Key_5,
                 Qt::Key_6,          Qt::Key_7,         Qt::Key_8,
                 Qt::Key_9,          Qt::Key_Period,    Qt::Key_Left,
                 Qt::Key_Right,      Qt::Key_Backspace, Qt::Key_ParenLeft,
                 Qt::Key_ParenRight, Qt::Key_Slash,     Qt::Key_Asterisk,
                 Qt::Key_Minus,      Qt::Key_Plus,      Qt::Key_AsciiCircum} {
  SetInitialView();
  ConnectButtonSignalsToSlots();
}

s21::View::~View() { delete ui_; }

double s21::View::GetGraphXMin() { return ui_->x_min->value(); }

double s21::View::GetGraphXMax() { return ui_->x_max->value(); }

s21::AngleUnit s21::View::GetAngleUnit() { return angle_unit_; }

double s21::View::GetGraphCountPoins() { return ui_->count_points->value(); }

char s21::View::GetGraphVar() {
  char result;
  if (ui_->comboBox->count())
    result =
        ui_->comboBox->itemText(ui_->comboBox->currentIndex()).at(0).toLatin1();
  else
    result = '\0';
  return result;
}

std::map<char, std::string> s21::View::GetVariables() {
  std::map<char, std::string> variables;
  if (ui_->input_x->isVisible())
    variables.emplace('x', ui_->input_x->toPlainText().toStdString());
  if (ui_->input_y->isVisible())
    variables.emplace('y', ui_->input_y->toPlainText().toStdString());
  if (ui_->input_z->isVisible())
    variables.emplace('z', ui_->input_z->toPlainText().toStdString());
  return variables;
}

std::string s21::View::GetScreenText() {
  return ui_->screen->text().toStdString();
}

void s21::View::Update(Info info) {
  if (ui_->input_x->isVisible())
    ui_->input_x->setText(QString::fromStdString(info.variables['x']));
  if (ui_->input_y->isVisible())
    ui_->input_y->setText(QString::fromStdString(info.variables['y']));
  if (ui_->input_z->isVisible())
    ui_->input_z->setText(QString::fromStdString(info.variables['z']));
  if (!info.error) ui_->screen->setText(QString::fromStdString(info.answer));
  ui_->message->setText(QString::fromStdString(info.message));
}

void s21::View::DrawGraph(std::vector<double> vector_x,
                          std::vector<double> vector_y) {
  QVector<double> vec_x = QVector<double>(vector_x.begin(), vector_x.end());
  QVector<double> vec_y = QVector<double>(vector_y.begin(), vector_y.end());
  ui_->widget->removeGraph(0);
  ui_->widget->addGraph();
  ui_->widget->graph(0)->setLineStyle((QCPGraph::LineStyle)QCPGraph::lsNone);
  ui_->widget->graph(0)->setScatterStyle(
      QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, Qt::white, 1));
  ui_->widget->graph(0)->addData(vec_x, vec_y);
  ui_->widget->replot();
}

bool s21::View::IsGraphMode() { return ui_->frame_graph->isVisible(); }

bool s21::View::IsFinMode() { return ui_->frame_fin->isVisible(); }

void s21::View::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape &&
      (qApp->focusWidget() == (QWidget *)ui_->deposits_list ||
       qApp->focusWidget() == (QWidget *)ui_->withdrawals_list ||
       qApp->focusWidget() == (QWidget *)ui_->deposit_date))
    ((QComboBox *)qApp->focusWidget())
        ->removeItem((((QComboBox *)qApp->focusWidget()))->currentIndex());
  else if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
           (event->modifiers() & Qt::AltModifier))
    on_button_message_clicked();
  else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    on_button_enter_clicked();
}

void s21::View::on_button_enter_clicked() { emit enter_clicked(); }

void s21::View::on_button_graph_clicked() {
  if (!ui_->frame_graph->isVisible()) {
    if (ui_->frame_fin->isVisible())
      ui_->frame_fin->hide();
    else
      setFixedSize(875, 600);
    ui_->frame_graph->show();
    on_screen_textChanged();
  } else {
    ui_->frame_graph->hide();
    setFixedSize(369, 600);
  }
  emit change_mode();
}

void s21::View::on_button_fin_clicked() {
  if (!ui_->frame_fin->isVisible()) {
    if (ui_->frame_graph->isVisible())
      ui_->frame_graph->hide();
    else
      setFixedSize(875, 600);
    ui_->frame_fin->show();
    on_screen_textChanged();
  } else {
    ui_->frame_fin->hide();
    setFixedSize(369, 600);
  }
  emit change_mode();
}

void s21::View::on_button_message_clicked() {
  ui_->screen->setText(ui_->message->toPlainText());
  on_button_enter_clicked();
}

void s21::View::on_message_textChanged() {
  int height = ui_->message->document()->size().height();
  if (height <= ui_->message->maximumHeight())
    ui_->message->setMinimumHeight(height);
  else
    ui_->message->setMinimumHeight(ui_->message->maximumHeight());
  if (ui_->message->toPlainText().size() > 300)
    ui_->message->setPlainText(ui_->message->toPlainText().remove(
        300, ui_->message->toPlainText().size() - 300));
  on_screen_textChanged();
}

void s21::View::on_screen_textChanged() {
  std::set<char> set;
  if (ui_->screen->text().contains('x') ||
      ui_->message->toPlainText().contains('x'))
    set.insert('x');
  if (ui_->screen->text().contains('y') ||
      ui_->message->toPlainText().contains('y'))
    set.insert('y');
  if (ui_->screen->text().contains('z') ||
      ui_->message->toPlainText().contains('z'))
    set.insert('z');
  MakeVisibleVariables(set, 'x', ui_->input_x);
  MakeVisibleVariables(set, 'y', ui_->input_y);
  MakeVisibleVariables(set, 'z', ui_->input_z);
}

void s21::View::on_button_rad_clicked() {
  QFont f = font();
  f.setUnderline(true);
  f.setBold(true);
  ui_->button_rad->setFont(f);
  f.setUnderline(false);
  f.setBold(false);
  ui_->button_deg->setFont(f);
  angle_unit_ = AngleUnit::kRadian;
  emit change_angle_unit(AngleUnit::kRadian);
}

void s21::View::on_button_deg_clicked() {
  QFont f = font();
  f.setUnderline(false);
  f.setBold(false);
  ui_->button_rad->setFont(f);
  f.setUnderline(true);
  f.setBold(true);
  ui_->button_deg->setFont(f);
  angle_unit_ = AngleUnit::kDegree;
  emit change_angle_unit(AngleUnit::kDegree);
}

void s21::View::keyboardButtonPressed() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  for (int i = 0; i < 25; ++i) {
    if (button_array_[i] == button) {
      EmitKey(key_array_[i]);
      return;
    }
  }
}

void s21::View::operationButtonPressed() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  if (ui_->screen->hasFocus())
    ui_->screen->insert(button->text());
  else if (ui_->message->hasFocus())
    ui_->message->insertPlainText(button->text());
}

void s21::View::setMaxLength() {
  QTextEdit *text_input = qobject_cast<QTextEdit *>(sender());
  if (text_input && text_input->toPlainText().length() > 100) {
    QString text = text_input->toPlainText();
    text = text.left(100);
    text_input->setPlainText(text);
    QTextCursor cursor = text_input->textCursor();
    cursor.setPosition(100);
    text_input->setTextCursor(cursor);
  }
}

void s21::View::on_comboBox_currentTextChanged() {
  if (ui_->comboBox->count())
    emit change_graph_var(ui_->comboBox->itemText(ui_->comboBox->currentIndex())
                              .at(0)
                              .toLatin1());
  else
    emit change_graph_var('\0');
}

void s21::View::on_x_min_editingFinished() {
  ui_->widget->xAxis->setRangeLower(ui_->x_min->value());
  emit change_graph_x_min(ui_->x_min->value());
}

void s21::View::on_x_max_editingFinished() {
  ui_->widget->xAxis->setRangeUpper(ui_->x_max->value());
  emit change_graph_x_max(ui_->x_max->value());
}

void s21::View::on_y_min_editingFinished() {
  ui_->widget->yAxis->setRangeLower(ui_->y_min->value());
  ui_->widget->replot();
}

void s21::View::on_y_max_editingFinished() {
  ui_->widget->yAxis->setRangeUpper(ui_->y_max->value());
  ui_->widget->replot();
}

void s21::View::on_x_min_valueChanged(double arg) {
  if (arg > ui_->x_max->value()) ui_->x_min->setValue(ui_->x_max->value());
}

void s21::View::on_x_max_valueChanged(double arg) {
  if (arg < ui_->x_min->value()) ui_->x_max->setValue(ui_->x_min->value());
}

void s21::View::on_y_min_valueChanged(double arg) {
  if (arg > ui_->y_max->value()) ui_->y_min->setValue(ui_->y_max->value());
}

void s21::View::on_y_max_valueChanged(double arg) {
  if (arg < ui_->y_min->value()) ui_->y_max->setValue(ui_->y_min->value());
}

void s21::View::on_count_points_valueChanged(int arg) {
  emit change_number_points(arg);
}

void s21::View::SetInitialView() {
  ui_->setupUi(this);
  ui_->frame_graph->hide();
  ui_->frame_fin->hide();
  ui_->input_x->hide();
  ui_->input_y->hide();
  ui_->input_z->hide();
  ui_->payment->setStyleSheet(
      "QComboBox { combobox-popup: 0; background-color "
      ": white; color : black; }");
  ui_->payment->setMaxVisibleItems(10);
  ui_->deposit_date->lineEdit()->setPlaceholderText("01.01.2000");
  ui_->deposits_list->lineEdit()->setPlaceholderText("01.01.2000 - 5000");
  ui_->withdrawals_list->lineEdit()->setPlaceholderText("01.01.2000 - 5000");
  QValidator *date_validator1 = new QRegularExpressionValidator(
      QRegularExpression("^([1-9]|[012][1-9]|[123][01])\\.([0]?[1-9]|[1][0-2])"
                         "\\.[12][0-9]{3}\\s?\\-\\s?[0-9]{1,9}$"),
      this);
  QValidator *date_validator2 = new QRegularExpressionValidator(
      QRegularExpression("^([1-9]|[012][1-9]|[123][01])\\.([0]?[1-9]|[1][0-2])"
                         "\\.[12][0-9]{3}$"),
      this);
  ui_->deposits_list->setValidator(date_validator1);
  ui_->withdrawals_list->setValidator(date_validator1);
  ui_->deposit_date->setValidator(date_validator2);
  setFixedSize(369, 600);
  ui_->screen->setFocus();
  ui_->widget->xAxis->setRange(ui_->x_min->value(), ui_->x_max->value());
  ui_->widget->yAxis->setRange(ui_->y_min->value(), ui_->y_max->value());
  // ui_->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
  // QCP::iSelectPlottables);
}

void s21::View::MakeVisibleVariables(std::set<char> set, char c,
                                     QTextEdit *widget) {
  if (set.find(c) != set.end()) {
    if (ui_->frame_graph->isVisible() &&
        ui_->comboBox->findText(QString(c)) == -1)
      ui_->comboBox->addItem(QString(c));
    if (!widget->isVisible()) widget->show();
  } else {
    int i = ui_->comboBox->findText(QString(c));
    if (ui_->frame_graph->isVisible() && i != -1) ui_->comboBox->removeItem(i);
    if (widget->isVisible()) widget->hide();
  }
}

void s21::View::EmitKey(Qt::Key key) {
  QWidget *receiver = qobject_cast<QWidget *>(QGuiApplication::focusObject());
  if (!receiver) return;
  QKeyEvent pressEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier,
                                   QKeySequence(key).toString());
  QKeyEvent releaseEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier);
  QCoreApplication::sendEvent(receiver, &pressEvent);
  QCoreApplication::sendEvent(receiver, &releaseEvent);
}

void s21::View::ConnectButtonSignalsToSlots() {
  for (int i = 0; i < 21; ++i) {
    QWidget *widget = ui_->gridLayout->itemAt(i)->widget();
    QPushButton *button = qobject_cast<QPushButton *>(widget);
    button_array_[i] = button;
    if (button)
      connect(button, SIGNAL(clicked()), this, SLOT(keyboardButtonPressed()));
  }
  for (int i = 21; i < 35; ++i) {
    QWidget *widget = ui_->gridLayout->itemAt(i)->widget();
    QPushButton *button = qobject_cast<QPushButton *>(widget);
    if (button)
      connect(button, SIGNAL(clicked()), this, SLOT(operationButtonPressed()));
  }
  connect(ui_->input_x, SIGNAL(textChanged()), this, SLOT(setMaxLength()));
  connect(ui_->input_y, SIGNAL(textChanged()), this, SLOT(setMaxLength()));
  connect(ui_->input_z, SIGNAL(textChanged()), this, SLOT(setMaxLength()));
}

void s21::View::SetPayment(std::vector<std::string> payment) {
  ui_->payment->clear();
  for (auto tmp : payment) ui_->payment->addItem(QString::fromStdString(tmp));
}

void s21::View::SetOverpayment(std::string overpayment) {
  ui_->overpayment->setText(QString::fromStdString(overpayment));
}

void s21::View::SetTotalAmount(std::string total_amount) {
  ui_->total_amount->setText(QString::fromStdString(total_amount));
}

void s21::View::on_button_credit_clicked() {
  CreditInfo credit_info;
  credit_info.credit_amount = ui_->credit_amount->value();
  credit_info.credit_period = ui_->period->value();
  credit_info.interest_rate = ui_->interest_rate->value();
  if (ui_->payment_type->currentIndex() == 0)
    credit_info.payment_type = PaymentType::kAnnuity;
  else
    credit_info.payment_type = PaymentType::kDifferentiated;
  if (ui_->period_unit->currentIndex() == 0)
    credit_info.period_type = PeriodType::kDays;
  else if (ui_->period_unit->currentIndex() == 1)
    credit_info.period_type = PeriodType::kMonths;
  else
    credit_info.period_type = PeriodType::kYears;
  emit calculate_credit(credit_info);
}

void s21::View::on_button_deposit_clicked() {
  DepositInfo deposit_info;
  deposit_info.deposit_amount = ui_->deposit_amount->value();
  deposit_info.deposit_date = ui_->deposit_date->currentText().toStdString();
  deposit_info.deposit_period = ui_->deposit_period->value();
  if (ui_->deposit_period_unit->currentIndex() == 0)
    deposit_info.period_type = PeriodType::kDays;
  else if (ui_->deposit_period_unit->currentIndex() == 1)
    deposit_info.period_type = PeriodType::kMonths;
  else
    deposit_info.period_type = PeriodType::kYears;
  if (ui_->payments_frequency->currentIndex() == 0)
    deposit_info.deposit_type = kDaily;
  else if (ui_->payments_frequency->currentIndex() == 1)
    deposit_info.deposit_type = kMonthly;
  else if (ui_->payments_frequency->currentIndex() == 2)
    deposit_info.deposit_type = kQuarterly;
  else
    deposit_info.deposit_type = kYearly;
  deposit_info.interest_rate = ui_->deposit_interest_rate->value();
  deposit_info.tax_rate = ui_->deposit_tax_rate->value();
  deposit_info.capitalization = ui_->capitalization->isChecked();
  for (int i = 0; i < ui_->deposits_list->count(); ++i)
    deposit_info.partial_deposits.push_back(
        ui_->deposits_list->itemText(i).toStdString());
  for (int i = 0; i < ui_->withdrawals_list->count(); ++i)
    deposit_info.partial_withdrawals.push_back(
        ui_->withdrawals_list->itemText(i).toStdString());
  emit calculate_deposit(deposit_info);
}

void s21::View::SetInterestFromDeposit(std::string interest) {
  ui_->interest_from_deposit->setText(QString::fromStdString(interest));
}

void s21::View::SetTaxAmount(std::string tax_amount) {
  ui_->tax_amount->setText(QString::fromStdString(tax_amount));
}

void s21::View::SetDepositTotalAmount(std::string total_amount) {
  ui_->deposit_total_amount->setText(QString::fromStdString(total_amount));
}
