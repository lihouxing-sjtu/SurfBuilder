#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
int main(int argc, char *argv[]) {

  QApplication::setStyle(QStyleFactory::create("Fusion"));
  QCoreApplication::setOrganizationName("Yingwei");
  QCoreApplication::setOrganizationDomain("http://www.med-3d.com/");
  QCoreApplication::setApplicationName("SurfBuilder");
  QApplication a(argc, argv);
  MainWindow w;
  w.showMaximized();

  return a.exec();
}
