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
  qDebug() << GG_FUNCTION << a.organizationDomain();
  qDebug() << GG_FUNCTION << a.organizationName();
  QSettings vSettings;
  qDebug() << GG_FUNCTION << vSettings.fileName();

  MainWindow w;
  w.show();

  return a.exec();
}
