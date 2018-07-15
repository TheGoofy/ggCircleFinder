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
#include "QtGraphics/ggGraphicsManipulatorRectItemT.h"



class ggGraphicsScene :
  public QGraphicsScene
{
public:

  typedef ggGraphicsManipulatorRectItemT<> tROI;

  ggGraphicsScene(QObject* aParent)
  : QGraphicsScene(aParent) {

    mImageCameraPixmapItem = new QGraphicsPixmapItem();
    addItem(mImageCameraPixmapItem);

    mImageHoughPixmapItem = new QGraphicsPixmapItem();
    addItem(mImageHoughPixmapItem);

    mROI = new tROI(QRectF(10,10,100,100));
    mROI->setPen(QPen(QColor(255, 200, 0), 0.0));
    mROI->SetHandleColor(QColor(255, 200, 0));
    addItem(mROI);
  }

  tROI& ROI() {
    return *mROI;
  }

  QSize GetROISize() const {
    return mROI->GetSize().toSize();
  }

  QPoint GetROIPosition() const {
    return (mROI->GetPosition() - QPointF(0.5, 0.5)).toPoint();
  }

  void PrepareCameraImage(const ggImageT<ggFloat>& aImageFloat,
                          ggFloat aCameraNoise,
                          ggInt32 aCameraNumberOfBits)
  {
    ggFloat vMin = 0.0f;
    ggFloat vMax = 0.0f;

    ggImageT<ggFloat> vImageFloat(aImageFloat);
    ggImageFilter::GetMinMax(vImageFloat, vMin, vMax);
    ggImageFilter::AddNoise(vImageFloat, aCameraNoise * (vMin != vMax ?  vMax - vMin : 1.0f));
    ggImageFilter::GetMinMax(vImageFloat, vMin, vMax);

    ggUInt16 vCameraValueMax = (1 << aCameraNumberOfBits) - 1;
    ggFloat vScaleCameraToDisplay = 255.0f / (ggFloat)vCameraValueMax;
    mImageCamera.Resize(vImageFloat.GetSizeX(), vImageFloat.GetSizeY());
    ggImageT<ggUChar> vImageUChar(vImageFloat.GetSizeX(), vImageFloat.GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < vImageFloat.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < vImageFloat.GetSizeX(); vIndexX++) {
        float vValue = (vImageFloat(vIndexX, vIndexY) - vMin) / (vMax - vMin);
        ggUInt16 vCameraValue = (ggInt16)(vCameraValueMax * vValue + 0.5f);
        mImageCamera(vIndexX, vIndexY) = vCameraValue;
        ggUChar vDisplayValue = (ggUChar)(vScaleCameraToDisplay * vCameraValue);
        vImageUChar(vIndexX, vIndexY) = vDisplayValue;
      }
    }

    QImage vImage(vImageUChar.GetValues(),
                  vImageUChar.GetSizeX(),
                  vImageUChar.GetSizeY(),
                  vImageUChar.GetSizeX(),
                  QImage::Format_Grayscale8);

    mImageCameraPixmapItem->setPixmap(QPixmap::fromImage(vImage));
  }

  void ComputeHoughImage(const bool aCircleModelGaussianFilter,
                         const ggFloat aCircleModelGaussianFilterWidth,
                         const ggFloat aCircleModelDiameter,
                         const ggFloat aCircleModelLineThickness,
                         const bool aCircleModelCenterVotesFilter,
                         const ggFloat aCircleModelCenterVotesFilterWidth)
  {
    // copy ROI image & smooth (remove the noise)
    qDebug() << "copy ROI image & smooth (remove the noise)";
    ggImageT<ggFloat> vImageCameraROI(GetROISize().width(), GetROISize().height());
    ggImageFilter::Copy(vImageCameraROI, mImageCamera, GetROIPosition().x(), GetROIPosition().y());
    if (aCircleModelGaussianFilter) ggImageFilter::Gauss(vImageCameraROI, aCircleModelGaussianFilterWidth);

    // calculate gradient vector field
    qDebug() << "calculate gradient vector field";
    ggImageT<ggVector2Float> vGradientVectorField(vImageCameraROI.GetSizeX(), vImageCameraROI.GetSizeY());
    ggImageFilter::Gradient(vGradientVectorField, vImageCameraROI);

    // do gradient based hough transformation
    qDebug() << "do gradient based hough transformation";
    ggImageT<ggFloat> vImageHough(vImageCameraROI.GetSize(), 0.0f);
    ggImagePainterT<ggFloat> vPainterHough(vImageHough);
    for (ggSize vIndexY = 1; vIndexY+1 < vGradientVectorField.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 1; vIndexX+1 < vGradientVectorField.GetSizeX(); vIndexX++) {
        const ggVector2Double vGradient(vGradientVectorField(vIndexX, vIndexY).GetConverted<ggDouble>());
        if (vGradient.Length() > 0.0f) {

          // try to fibure out some sort of circle curvature depending on the neighbor gradients.
          // - triangulate a virtual center-point (accuracy might be a problem due to the close
          //   proximity of the two triangulation points). nevertheless the virtual center can be used
          //   as indicator, on which side to expect the circle center
          // - the two triangulation points are selected perpendicular to the actual gradient
          bool vDiagonalQ1 = vGradient.X() * vGradient.Y() > 0.0f;
          bool vHorizontal = fabs(vGradient.X()) > fabs(vGradient.Y());
          ggDouble vSlope = vHorizontal ? (vGradient.Y() / vGradient.X()) : (vGradient.X() / vGradient.Y());
          // position of two (intersecting) lines
          ggVector2Int32 vIndexA(vIndexX, vIndexY);
          ggVector2Int32 vIndexB(vIndexX, vIndexY);
          if (fabs(vSlope) < 0.4142) { // tan(22.5) = sqrt(2) - 1
            vIndexA += ggVector2Int32(vHorizontal ? 0 : 1, vHorizontal ? 1 : 0);
            vIndexB += ggVector2Int32(vHorizontal ? 0 : -1, vHorizontal ? -1 : 0);
          }
          else {
            vIndexA += ggVector2Int32(vDiagonalQ1 ? -1 : 1, vDiagonalQ1 ? 1 : 1);
            vIndexB += ggVector2Int32(vDiagonalQ1 ? 1 : -1, vDiagonalQ1 ? -1 : -1);
          }
          // direction of two (intersecting) lines
          const ggVector2Double vGradientA(vGradientVectorField(vIndexA).GetConverted<ggDouble>());
          const ggVector2Double vGradientB(vGradientVectorField(vIndexB).GetConverted<ggDouble>());
          // two lines potentially intersect at the virtual center-point
          ggLine2Double vLineA = ggLine2Double::FromDir(vIndexA.GetConverted<ggDouble>(), vGradientA);
          ggLine2Double vLineB = ggLine2Double::FromDir(vIndexB.GetConverted<ggDouble>(), vGradientB);
          ggVector2Double vIntersection;
          // if the two lines are not parallel, we'll get an intersection
          if (ggGeometry::Intersect(vLineA, vLineB, vIntersection)) {
            // point on circle
            ggVector2Double vCirclePoint(vIndexX, vIndexY);
            // direction to the virtual center-point (if the circle radius was unknown, we could use this as an estimate)
            ggVector2Double vCenterDirection(vIntersection - vCirclePoint);
            // (known) radius needs to be negative, when the gradient points outward the circle
            ggDouble vRadius = vCenterDirection.Dot(vGradient) > 0.0 ? aCircleModelDiameter / 2.0 : -aCircleModelDiameter / 2.0;
            // use the actual gradient (and not the gradient from neighbors) for the center voting
            vCenterDirection = vRadius * vGradient.Normalized();
            // the position of the center point may varies by the thickenss of the circle line
            ggVector2Double vRadiusRange(0.5f * aCircleModelLineThickness * vGradient.Normalized());
            // if the length of the gradient is high, it's more likely we've detected some reasonable structures (not noise)
            ggFloat vIntensity = vGradient.Length();
            // voting for center point candidates on one side of the (potential) circle point ...
            vPainterHough.DrawLine(vCirclePoint + vCenterDirection - vRadiusRange,
                                   vCirclePoint + vCenterDirection + vRadiusRange,
                                   vIntensity, 1.0f,  ggPainterBlendType::eAdd);
          }

        }
      }
    }

    // smooth hough image
    qDebug() << "smooth hough voting image";
    if (aCircleModelCenterVotesFilter) ggImageFilter::Gauss(vImageHough, aCircleModelCenterVotesFilterWidth);
/*
    // detect center spots by finding all local maxima
    qDebug() << "detect center spots by finding all local maxima";
    typedef ggSpotT<ggFloat, ggVector2Double> tSpot;
    typedef std::vector<tSpot> tSpots;
    tSpots vCenterSpots = ggImageFilter::FindLocalMaxima(vImageHough, true);

    // sort center spots from highest to lowest
    qDebug() << "sort center spots from highest to lowest";
    std::sort(vCenterSpots.begin(), vCenterSpots.end(),
              [] (const tSpot& aSpotA, const tSpot& aSpotB) {return aSpotA.GetValue() > aSpotB.GetValue();} );
*/
    // convert hough image for rendering with QT
    qDebug() << "convert hough image for rendering with QT";
    ggImageT<ggUChar> vImageUChar = vImageHough.GetConvertedFitMinMax<ggUChar>();

    QImage vImage(vImageUChar.GetValues(),
                  vImageUChar.GetSizeX(),
                  vImageUChar.GetSizeY(),
                  vImageUChar.GetSizeX(),
                  QImage::Format_Indexed8);

    // generate a nice color table for the hough image
    qDebug() << "generate a nice color table for the hough image";
    QVector<QRgb> vColorTable(256);
    for (int vIndex = 0; vIndex < vColorTable.size(); vIndex++) {
      if      (vIndex <   4) vColorTable[vIndex] = QColor(2*vIndex,          0,             8*vIndex,       64*vIndex).rgba();
      else if (vIndex <  32) vColorTable[vIndex] = QColor(2*vIndex,          0,             8*vIndex,       255      ).rgba();
      else if (vIndex <  64) vColorTable[vIndex] = QColor(2*vIndex,          0,             255,            255      ).rgba();
      else if (vIndex <  96) vColorTable[vIndex] = QColor(3*(vIndex-64)+128, 0,             4*(127-vIndex), 255      ).rgba();
      else if (vIndex < 128) vColorTable[vIndex] = QColor(1*(vIndex-96)+224, 2*(vIndex-96), 4*(127-vIndex), 255      ).rgba();
      else if (vIndex < 160) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 0,              255      ).rgba();
      else if (vIndex < 192) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 0,              255      ).rgba();
      else if (vIndex < 224) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 4*(vIndex-192), 255      ).rgba();
      else                   vColorTable[vIndex] = QColor(255,               255,           4*(vIndex-192), 255      ).rgba();
      // for (int vX = 0; vX < 50; vX++) vImageUChar(vX, vIndex) = vIndex;
    }
    vImage.setColorTable(vColorTable);

    // display the results
    mImageHoughPixmapItem->setPos(GetROIPosition());
    mImageHoughPixmapItem->setPixmap(QPixmap::fromImage(vImage));
  }

  void SetHoughImageOpacity(ggFloat aOpacity)
  {
    mImageHoughPixmapItem->setOpacity(aOpacity);
  }

private:

  tROI* mROI;

  ggImageT<ggUInt16> mImageCamera;
  QGraphicsPixmapItem* mImageCameraPixmapItem;

  QGraphicsPixmapItem* mImageHoughPixmapItem;

};


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

  mScene->ComputeHoughImage(vCircleModelGaussianFilter,
                            vCircleModelGaussianFilterWidth,
                            vCircleModelDiameter,
                            vCircleModelLineThickness,
                            vCircleModelCenterVotesFilter,
                            vCircleModelCenterVotesFilterWidth);

  on_mOverlayOpacitySlider_valueChanged(UI().mOverlayOpacitySlider->value());
}


void MainWindow::on_mOverlayOpacitySlider_valueChanged(int /*aValue*/)
{
  const ggInt32 vOpacitySliderValue = UI().mOverlayOpacitySlider->value();
  const ggInt32 vOpacitySliderValueMin = UI().mOverlayOpacitySlider->minimum();
  const ggInt32 vOpacitySliderValueMax = UI().mOverlayOpacitySlider->maximum();

  const ggFloat vOpacity = (ggFloat)(vOpacitySliderValue - vOpacitySliderValueMin) /
                           (ggFloat)(vOpacitySliderValueMax - vOpacitySliderValueMin);

  mScene->SetHoughImageOpacity(vOpacity);
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
