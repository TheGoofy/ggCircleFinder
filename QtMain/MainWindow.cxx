#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>

#include "LibBase/ggGeometry.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggImagePainterT.h"
#include "LibImage/ggImageAlgorithm.h"
#include "QtBase/ggUtilityQt.h"
#include "QtGraphics/ggGraphicsManipulatorRectItemT.h"
#include "QtMain/ggGraphicsScene.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  mUserInterface(new Ui::MainWindow)
{
  UI().setupUi(this);
  centralWidget()->layout()->setMargin(0);
  centralWidget()->layout()->setSpacing(0);

  ReadSettings();

  UI().toolBar->hide();
  UI().statusBar->setContentsMargins(0, -6, 0, -6);
  UI().statusBar->addPermanentWidget(UI().mZoomLabel);
  UI().statusBar->addPermanentWidget(UI().mZoomComboBox);
  UI().statusBar->addPermanentWidget(UI().mZoomFitPushButton);
  UI().statusBar->addPermanentWidget(UI().mZoomResetPushButton);
  UI().centralWidget->layout()->setMargin(0);
  UI().centralWidget->layout()->setSpacing(0);
  UI().mZoomComboBox->setCompleter(0);
  UI().mZoomComboBox->setFocusPolicy(Qt::ClickFocus);

  connect(UI().mZoomComboBox->lineEdit(), SIGNAL(editingFinished()), this, SLOT(on_mZoomComboBox_editingFinished()));

  mScene = new ggGraphicsScene(this);
  UI().mGraphicsView->setScene(mScene);

  // register subject(s)
  Attach(UI().mGraphicsView->GetSubjectZoom());
  Attach(mScene->ROI().GetSubjectPosition());
  Attach(mScene->ROI().GetSubjectSize());

  UpdateUserInterfaceZoom();
}


MainWindow::~MainWindow()
{
  delete mUserInterface;
}


Ui::MainWindow& MainWindow::UI()
{
  return *mUserInterface;
}


void MainWindow::UpdateROI()
{
  ggObserver::cExecutorBlocking vBlockPosition(this, mScene->ROI().GetSubjectPosition());
  ggObserver::cExecutorBlocking vBlockSize(this, mScene->ROI().GetSubjectSize());
  mScene->ROI().SetPosition(QPointF(UI().mRegionOfInterestPositionXLineEdit->text().toInt(),
                                    UI().mRegionOfInterestPositionYLineEdit->text().toInt()));
  mScene->ROI().SetSize(QSizeF(UI().mRegionOfInterestSizeXLineEdit->text().toInt(),
                               UI().mRegionOfInterestSizeYLineEdit->text().toInt()));
}


void MainWindow::UpdateUserInterfaceZoom()
{
  UI().mZoomComboBox->lineEdit()->setText(ZoomToString(UI().mGraphicsView->GetSubjectZoom()->GetValue()));
}


void MainWindow::UpdateUserInterfaceROI()
{
  UI().mRegionOfInterestPositionXLineEdit->setText(QString::number(mScene->ROI().GetPosition().x(), 'f', 0));
  UI().mRegionOfInterestPositionYLineEdit->setText(QString::number(mScene->ROI().GetPosition().y(), 'f', 0));
  UI().mRegionOfInterestSizeXLineEdit->setText(QString::number(mScene->ROI().GetSize().width(), 'f', 0));
  UI().mRegionOfInterestSizeYLineEdit->setText(QString::number(mScene->ROI().GetSize().height(), 'f', 0));
}


void MainWindow::Update(const ggSubject* aSubject)
{
  if (aSubject == UI().mGraphicsView->GetSubjectZoom()) {
    UpdateUserInterfaceZoom();
  }

  if (aSubject == mScene->ROI().GetSubjectPosition() ||
      aSubject == mScene->ROI().GetSubjectSize()) {
    UpdateUserInterfaceROI();
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
  float vZoomFloat = StringToZoom(UI().mZoomComboBox->itemText(aIndex));
  UI().mGraphicsView->GetSubjectZoom()->SetValue(vZoomFloat);
}


void MainWindow::on_mZoomComboBox_editingFinished()
{
  float vZoomFloat = StringToZoom(UI().mZoomComboBox->lineEdit()->text());
  if (vZoomFloat > 0.0f) UI().mGraphicsView->GetSubjectZoom()->SetValue(vZoomFloat);
}


void MainWindow::on_mZoomFitPushButton_clicked()
{
  UI().mGraphicsView->SetZoomFit();
}


void MainWindow::on_mZoomResetPushButton_clicked()
{
  UI().mGraphicsView->SetZoomReset();
}


void MainWindow::on_mRegionOfInterestPositionXLineEdit_editingFinished()
{
  UpdateROI();
}


void MainWindow::on_mRegionOfInterestPositionYLineEdit_editingFinished()
{
  UpdateROI();
}


void MainWindow::on_mRegionOfInterestSizeXLineEdit_editingFinished()
{
  UpdateROI();
}


void MainWindow::on_mRegionOfInterestSizeYLineEdit_editingFinished()
{
  UpdateROI();
}


void MainWindow::on_mGenerateImagePushButton_clicked()
{
  // settings from user interface
  const ggSize vCameraImageSizeX = UI().mCameraImageSizeXLineEdit->text().toUInt();
  const ggSize vCameraImageSizeY = UI().mCameraImageSizeYLineEdit->text().toUInt();
  const ggFloat vCircleCenterX = UI().mCircleCenterXLineEdit->text().toFloat();
  const ggFloat vCircleCenterY = UI().mCircleCenterYLineEdit->text().toFloat();
  const ggFloat vCircleDiameter = UI().mCircleDiameterLineEdit->text().toFloat();
  const ggFloat vCircleLineThickness = UI().mCircleLineThicknessLineEdit->text().toFloat();
  const ggFloat vCircleLineFragmentation = UI().mCircleLineFragmentationSpinBox->value() / 100.0f;
  const ggFloat vCameraNoise = UI().mCameraNoiseDoubleSpinBox->value() / 100.0f;
  const ggInt32 vCameraNumberOfBits = UI().mCameraBitDepthSpinBox->value();

  if (vCameraImageSizeX == 0) return;
  if (vCameraImageSizeY == 0) return;

  ggImageT<ggFloat> vImageFloat(vCameraImageSizeX, vCameraImageSizeY);
  ggImagePainterT<ggFloat> vImagePainter(vImageFloat);

  // init image
  vImageFloat = 0.0f;

  // draw a circle
  ggVector2Double vCenter(vCircleCenterX, vCircleCenterY);
  ggDouble vRadius(vCircleDiameter / 2.0f);
  ggInt32 vNumSteps = std::max<ggInt32>(1, vCircleLineThickness);
  ggDouble vStepLength = vCircleLineThickness / vNumSteps;
  for (ggInt32 vStep = 0; vStep < vNumSteps; vStep++) {
    vImagePainter.DrawCircle(vCenter, vRadius - (vNumSteps - 1) * vStepLength / 2.0 + vStep * vStepLength,
                             1.0f, 1.0f, ggPainterBlendType::eSet, vCircleLineFragmentation);
  }

  // calculate camera image and image for rendering on screen
  mScene->PrepareCameraImage(vImageFloat,
                             vCameraNoise,
                             vCameraNumberOfBits);

  UI().mGraphicsView->SetZoomFit();
}


void MainWindow::on_mLoadImagePushButton_clicked()
{
  QFileDialog vFileDialog(this);
  if (vFileDialog.exec()) {

    QPixmap vPixmap(vFileDialog.selectedFiles().front());
    if (!vPixmap.isNull()) {

      // convert image to grayscale float
      QImage vImage = vPixmap.toImage();
      ggImageT<ggFloat> vImageFloat(vImage.width(), vImage.height());
      for (ggSize vIndexY = 0; vIndexY < vImageFloat.GetSizeY(); vIndexY++) {
        for (ggSize vIndexX = 0; vIndexX < vImageFloat.GetSizeX(); vIndexX++) {
          vImageFloat(vIndexX, vIndexY) = vImage.pixelColor(vIndexX, vIndexY).lightnessF();
        }
      }

      // settings from user interface
      const ggFloat vCameraNoise = UI().mCameraNoiseDoubleSpinBox->value() / 100.0f;
      const ggInt32 vCameraNumberOfBits = UI().mCameraBitDepthSpinBox->value();

      // calculate camera image and image for rendering on screen
      mScene->PrepareCameraImage(vImageFloat,
                                 vCameraNoise,
                                 vCameraNumberOfBits);

      UI().mGraphicsView->SetZoomFit();
    }
  }
}


void MainWindow::on_mFindCirclesPushButton_clicked()
{
  // read parameters from GUI
  const ggVector2Float vOriginalCircleCenter(UI().mCircleCenterXLineEdit->text().toFloat(),
                                             UI().mCircleCenterYLineEdit->text().toFloat());
  const ggFloat vOriginalCircleDiameter = UI().mCircleDiameterLineEdit->text().toFloat();
  const ggFloat vCircleThickness = UI().mCircleLineThicknessLineEdit->text().toFloat();
  const bool vCircleModelGaussianFilter = UI().mCircleModelGaussianFilterCheckBox->isChecked();
  const ggFloat vCircleModelGaussianFilterWidth = UI().mCircleModelGaussianFilterWidthLineEdit->text().toFloat();
  const ggFloat vCircleModelDiameter = UI().mCircleModelDiameterLineEdit->text().toFloat();
  const ggFloat vCircleModelLineThickness = UI().mCircleModelLineThicknessLineEdit->text().toFloat();
  const bool vCircleModelCenterVotesFilter = UI().mCircleModelCenterVotesFilterCheckBox->isChecked();
  const ggFloat vCircleModelCenterVotesFilterWidth = UI().mCircleModelCenterVotesFilterWidthLineEdit->text().toFloat();
  const ggInt32 vCircleModelNumberOfCircles = UI().mCircleModelNumberOfCirclesSpinBox->value();

  QString vResults = mScene->FindCircles(vCircleModelGaussianFilter,
                                         vCircleModelGaussianFilterWidth,
                                         vCircleModelDiameter,
                                         vCircleModelLineThickness,
                                         vCircleModelCenterVotesFilter,
                                         vCircleModelCenterVotesFilterWidth,
                                         vCircleModelNumberOfCircles);

  UI().mResultsPlainTextEdit->setPlainText(vResults);

  on_mOverlayOpacitySlider_valueChanged(UI().mOverlayOpacitySlider->value());
  on_mCirclesOpacitySlider_valueChanged(UI().mCirclesOpacitySlider->value());
}


void MainWindow::on_mOverlayOpacitySlider_valueChanged(int /*aValue*/)
{
  const ggInt32 vValue = UI().mOverlayOpacitySlider->value();
  const ggInt32 vValueMin = UI().mOverlayOpacitySlider->minimum();
  const ggInt32 vValueMax = UI().mOverlayOpacitySlider->maximum();

  const ggFloat vOpacity = (ggFloat)(vValue - vValueMin) /
                           (ggFloat)(vValueMax - vValueMin);

  mScene->SetHoughImageOpacity(vOpacity);
}


void MainWindow::on_mCirclesOpacitySlider_valueChanged(int /*aValue*/)
{
  const ggInt32 vValue = UI().mCirclesOpacitySlider->value();
  const ggInt32 vValueMin = UI().mCirclesOpacitySlider->minimum();
  const ggInt32 vValueMax = UI().mCirclesOpacitySlider->maximum();

  const ggFloat vOpacity = (ggFloat)(vValue - vValueMin) /
                           (ggFloat)(vValueMax - vValueMin);

  mScene->SetCirclesOpacity(vOpacity);
}


void MainWindow::closeEvent(QCloseEvent* aEvent)
{
  WriteSettings();
  aEvent->accept();
}


void MainWindow::WriteSettings()
{
  QSettings vSettings;
  vSettings.beginGroup("MainWindow");
  vSettings.setValue("size", size());
  vSettings.setValue("pos", pos());
  vSettings.endGroup();
}


void MainWindow::ReadSettings()
{
  QSettings vSettings;
  vSettings.beginGroup("MainWindow");
  resize(vSettings.value("size", size()).toSize());
  move(vSettings.value("pos", pos()).toPoint());
  vSettings.endGroup();
}
