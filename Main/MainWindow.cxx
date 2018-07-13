#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  centralWidget()->layout()->setMargin(0);
  centralWidget()->layout()->setSpacing(0);
}


MainWindow::~MainWindow()
{
  delete ui;
}


Ui::MainWindow* MainWindow::GetUI()
{
  return ui;
}
