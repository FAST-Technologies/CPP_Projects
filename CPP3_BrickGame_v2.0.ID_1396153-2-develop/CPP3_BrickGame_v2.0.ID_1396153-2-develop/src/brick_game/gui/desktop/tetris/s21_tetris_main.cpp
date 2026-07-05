#include <QApplication>
#include <QDir>
#include <QStyleFactory>

#include "s21_tetris_view.h"

int main(int argc, char* argv[]) {
  s21::set_interface_mode(true);
  QApplication app(argc, argv);
  app.setApplicationName("Tetris Game");
  app.setApplicationVersion("2.0");
  app.setOrganizationName("School 21");
  app.setStyle(QStyleFactory::create("Fusion"));
  s21::TetrisView window;
  window.show();
  return app.exec();
}