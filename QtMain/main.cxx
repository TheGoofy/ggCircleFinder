#include "MainWindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "LibBase/ggRoundTest.h"
#include "LibImage/ggHistogramTest.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("Goofy Soft");
  QCoreApplication::setApplicationName("ggCircleFinder");
  qDebug() << __PRETTY_FUNCTION__ << a.organizationDomain();
  qDebug() << __PRETTY_FUNCTION__ << a.organizationName();
  QSettings vSettings;
  qDebug() << __PRETTY_FUNCTION__ << vSettings.fileName();

  ggRoundTest::Execute();
  ggHistogramTest::Execute();

  MainWindow w;
  w.show();

  return a.exec();
}
