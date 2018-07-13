#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  centralWidget()->layout()->setMargin(0);
  centralWidget()->layout()->setSpacing(0);

  ui->toolBar->hide();
  ui->statusBar->setContentsMargins(0, -6, 0, -6);
  ui->statusBar->addPermanentWidget(ui->mZoomLabel);
  ui->statusBar->addPermanentWidget(ui->mZoomComboBox);
  ui->statusBar->addPermanentWidget(ui->mZoomFitPushButton);
  ui->statusBar->addPermanentWidget(ui->mZoomResetPushButton);
  ui->centralWidget->layout()->setMargin(0);
  ui->centralWidget->layout()->setSpacing(0);
  ui->mZoomComboBox->setCompleter(0);
  ui->mZoomComboBox->setFocusPolicy(Qt::ClickFocus);

  connect(ui->mZoomComboBox->lineEdit(), SIGNAL(editingFinished()), this, SLOT(on_mZoomComboBox_editingFinished()));

  QGraphicsScene* vScene = new QGraphicsScene(this);
  vScene->addEllipse(300, 100, 200, 150);
  ui->mGraphicsView->setScene(vScene);

  // register subject(s)
  Attach(ui->mGraphicsView->GetSubjectZoom());

  UpdateUI();
}


MainWindow::~MainWindow()
{
  delete ui;
}


Ui::MainWindow* MainWindow::GetUI()
{
  return ui;
}


void MainWindow::UpdateUI()
{
  ui->mZoomComboBox->lineEdit()->setText(ZoomToString(ui->mGraphicsView->GetSubjectZoom()->GetValue()));
}


void MainWindow::Update(const ggSubject* aSubject)
{
  if (aSubject == ui->mGraphicsView->GetSubjectZoom()) {
    UpdateUI();
  }
}


QString MainWindow::ZoomToString(float aZoomFloat) const
{
  return QString::number((int)(100.0f*aZoomFloat + 0.5f)) + "%";
}


float MainWindow::StringToZoom(const QString& aZoomString) const
{
  QStringList vZoomStrings = aZoomString.split("%");
  float vZoomFloat = vZoomStrings.first().toFloat() / 100.0f;
  return vZoomFloat;
}


void MainWindow::on_mZoomComboBox_activated(int aIndex)
{
  float vZoomFloat = StringToZoom(ui->mZoomComboBox->itemText(aIndex));
  ui->mGraphicsView->GetSubjectZoom()->SetValue(vZoomFloat);
}


void MainWindow::on_mZoomComboBox_editingFinished()
{
  float vZoomFloat = StringToZoom(ui->mZoomComboBox->lineEdit()->text());
  if (vZoomFloat > 0.0f) ui->mGraphicsView->GetSubjectZoom()->SetValue(vZoomFloat);
}


void MainWindow::on_mZoomFitPushButton_clicked()
{
  ui->mGraphicsView->SetZoomFit();
}


void MainWindow::on_mZoomResetPushButton_clicked()
{
  ui->mGraphicsView->SetZoomReset();
}


