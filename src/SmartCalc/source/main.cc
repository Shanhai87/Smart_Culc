#include <QApplication>

#include "../include/controller.h"
#include "../include/model.h"
#include "../include/view.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  s21::View view;
  s21::Model model;
  s21::Controller controller(&view, &model);
  view.show();
  return a.exec();
}
