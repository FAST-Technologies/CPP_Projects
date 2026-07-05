#include <QApplication>
#include <QCoreApplication>
#include <QSurfaceFormat>

#include "controller/controller.h"
#include "view/view.h"

using namespace s21;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("S21");
  QCoreApplication::setApplicationName("3DViewer v2.0");

  QSurfaceFormat format;
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  format.setSamples(4);
  QSurfaceFormat::setDefaultFormat(format);

  View view;
  Controller controller(&view);
  view.show();

  return app.exec();
}