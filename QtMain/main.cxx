#include "MainWindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "LibBase/ggUnitTest.h"

int main(int argc, char *argv[])
{
  ggUnitTest::ExecuteAll();

  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("Goofy Soft");
  QCoreApplication::setApplicationName("ggCircleFinder");
  qDebug() << __PRETTY_FUNCTION__ << a.organizationDomain();
  qDebug() << __PRETTY_FUNCTION__ << a.organizationName();
  QSettings vSettings;
  qDebug() << __PRETTY_FUNCTION__ << vSettings.fileName();

  MainWindow w;
  w.show();

  return a.exec();
}
