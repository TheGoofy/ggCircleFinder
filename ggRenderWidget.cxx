#include "ggRenderWidget.h"
#include <iostream>
#include <QPainter>
#include <QImage>
#include <QBitmap>
#include <QElapsedTimer>
#include <QFileDialog>
#include "ggImageT.h"
#include "ggImageFilter.h"
#include "ggRunningAverages.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ggLinear.h"
#include "ggImagePainterT.h"
#include "ggGeometry.h"
#include <QDebug>


ggRenderWidget::ggRenderWidget(QWidget *parent) :
  QFrame(parent),
  mImageCamera(),
  mImagePixmap()
{
}


void ggRenderWidget::PrepareCameraImage(const ggImageT<ggFloat>& aImageFloat)
{
  MainWindow* vMainWindow = dynamic_cast<MainWindow*>(aParent()->aParent());
  Ui::MainWindow* vUI = vMainWindow->GetUI();

  const ggFloat vCameraNoise = vUI->mCameraNoiseDoubleSpinBox->value() / 100.0f;
  const ggInt32 vCameraNumberOfBits = vUI->mCameraBitDepthSpinBox->value();

  ggFloat vMin = 0.0f;
  ggFloat vMax = 0.0f;

  ggImageT<ggFloat> vImageFloat(aImageFloat);
  ggImageFilter::GetMinMax(vImageFloat, vMin, vMax);
  ggImageFilter::AddNoise(vImageFloat, vCameraNoise * (vMin != vMax ?  vMax - vMin : 1.0f));
  ggImageFilter::GetMinMax(vImageFloat, vMin, vMax);

  ggUInt16 vCameraValueMax = (1 << vCameraNumberOfBits) - 1;
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

  mImagePixmap = QPixmap::fromImage(vImage);

  QPainter vPainter(&mImagePixmap);
  vPainter.setRenderHint(QPainter::Antialiasing, false);
  vPainter.setRenderHint(QPainter::SmoothPixmapTransform, false);

  mOverlayPixmap = QPixmap(mImageCamera.GetSizeX(), mImageCamera.GetSizeY());
  mOverlayPixmap.fill(Qt::transparent);
}


void ggRenderWidget::LoadImage()
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

      // calculate camera image and image for rendering on screen
      PrepareCameraImage(vImageFloat);

      repaint();
    }
  }
}


void ggRenderWidget::GenerateImage()
{
  MainWindow* vMainWindow = dynamic_cast<MainWindow*>(aParent()->aParent());
  Ui::MainWindow* vUI = vMainWindow->GetUI();

  const ggSize vCameraImageSizeX = vUI->mCameraImageSizeXLineEdit->text().toUInt();
  const ggSize vCameraImageSizeY = vUI->mCameraImageSizeYLineEdit->text().toUInt();
  const ggFloat vLineBeginX = vUI->mLineBeginXLineEdit->text().toFloat();
  const ggFloat vLineBeginY = vUI->mLineBeginYLineEdit->text().toFloat();
  const ggFloat vLineEndX = vUI->mLineEndXLineEdit->text().toFloat();
  const ggFloat vLineEndY = vUI->mLineEndYLineEdit->text().toFloat();
  const ggFloat vCircleCenterX = vUI->mCircleCenterXLineEdit->text().toFloat();
  const ggFloat vCircleCenterY = vUI->mCircleCenterYLineEdit->text().toFloat();
  const ggFloat vCircleDiameter = vUI->mCircleDiameterLineEdit->text().toFloat();
  const ggFloat vLineThickness = vUI->mLineThicknessLineEdit->text().toFloat();
  const ggFloat vLineFragmentation = vUI->mLineFragmentationSpinBox->value() / 100.0f;

  if (vCameraImageSizeX == 0) return;
  if (vCameraImageSizeY == 0) return;

  ggImageT<ggFloat> vImageFloat(vCameraImageSizeX, vCameraImageSizeY);
  ggImagePainterT<ggFloat> vImagePainter(vImageFloat);

  // init image
  vImageFloat = 0.0f;

  // draw the laser line
  ggVector2Double vLaserLineBegin(vLineBeginX, vLineBeginY);
  ggVector2Double vLaserLineEnd(vLineEndX, vLineEndY);
  vImagePainter.DrawLine(vLaserLineBegin, vLaserLineEnd,
                         1.0f, 1.0f, ggPainterBlendType::eSet, vLineFragmentation);

  // draw a circle
  ggVector2Double vCenter(vCircleCenterX, vCircleCenterY);
  ggDouble vRadius(vCircleDiameter / 2.0f);
  for (ggSize vIndex = 0; vIndex < 50; vIndex++) {
    vImagePainter.DrawCircle(vCenter, vRadius + vIndex,
                             1.0f, 1.0f, ggPainterBlendType::eSet, vLineFragmentation);
  }

  // blur the lines
  if (vLineThickness != 0.0) ggImageFilter::Gauss(vImageFloat, vLineThickness);

  // calculate camera image and image for rendering on screen
  PrepareCameraImage(vImageFloat);

  repaint();
}


void ggRenderWidget::FindLine()
{
  MainWindow* vMainWindow = dynamic_cast<MainWindow*>(aParent()->aParent());
  Ui::MainWindow* vUI = vMainWindow->GetUI();

  const ggFloat vLineBeginX = vUI->mLineBeginXLineEdit->text().toFloat();
  const ggFloat vLineBeginY = vUI->mLineBeginYLineEdit->text().toFloat();
  const ggFloat vLineEndX = vUI->mLineEndXLineEdit->text().toFloat();
  const ggFloat vLineEndY = vUI->mLineEndYLineEdit->text().toFloat();
  const ggFloat vLineThickness = vUI->mLineThicknessLineEdit->text().toFloat();
  const ggInt32 vRegionOfInterestPositionX = vUI->mRegionOfInterestPositionXLineEdit->text().toInt();
  const ggInt32 vRegionOfInterestPositionY = vUI->mRegionOfInterestPositionYLineEdit->text().toInt();
  const ggInt32 vRegionOfInterestSizeX = vUI->mRegionOfInterestSizeXLineEdit->text().toInt();
  const ggInt32 vRegionOfInterestSizeY = vUI->mRegionOfInterestSizeYLineEdit->text().toInt();
  const ggFloat vLineModelThickness = vUI->mLineModelThicknessLineEdit->text().toFloat();
  const ggFloat vLineModelFragmentation = vUI->mLineModelFragmentationSpinBox->value() / 100.0f;
  const bool vLineModelGaussianFilter = vUI->mLineModelGaussianFilterCheckBox->isChecked();

  // start a timer
  QElapsedTimer vTimer;
  vTimer.start();

  // copy image & smooth (remove the noise)
  ggImageT<ggUInt16> vImageCameraROI(vRegionOfInterestSizeX, vRegionOfInterestSizeY);
  ggImageFilter::Copy(vImageCameraROI, mImageCamera, vRegionOfInterestPositionX, vRegionOfInterestPositionY);
  if (vLineModelGaussianFilter) ggImageFilter::Gauss(vImageCameraROI, vLineModelThickness);

  // calculate a histogram
  std::vector<ggSize> vHistogram = ggImageFilter::GetHistogram(vImageCameraROI);

  // calculate the threshold
  ggSize vHistogramMax = 0;
  ggSize vRegionPixelCount = vImageCameraROI.GetSizeX() * vImageCameraROI.GetSizeY();
  ggSize vLinePixelCount = vImageCameraROI.GetSizeX() * std::min(vLineModelThickness*(1.0f-vLineModelFragmentation), (ggFloat)vImageCameraROI.GetSizeY());
  ggSize vBackgroundPixelCount = vRegionPixelCount - vLinePixelCount;
  ggUInt16 vLineThresholdValue = 0;
  ggSize vTotalPixelCount = 0;
  for (ggUInt32 vValue = 0; vValue < vHistogram.size(); vValue++) {
    vTotalPixelCount += vHistogram[vValue];
    if (vTotalPixelCount < vBackgroundPixelCount) vLineThresholdValue = vValue;
    if (vHistogram[vValue] > vHistogramMax) vHistogramMax = vHistogram[vValue];
  }

  // calculate center of gravity
  std::vector<ggUInt16> vPositionsX;
  std::vector<ggUInt16> vPositionsY;
  ggRunningAveragesT<ggFloat> vAveragesX;
  ggRunningAveragesT<ggFloat> vAveragesY;
  for (ggInt32 vIndexY = 0; vIndexY < (ggInt32)vImageCameraROI.GetSizeY(); vIndexY++) {
    for (ggInt32 vIndexX = 0; vIndexX < (ggInt32)vImageCameraROI.GetSizeX(); vIndexX++) {
      ggUInt16 vValue = vImageCameraROI(vIndexX, vIndexY);
      if (vValue > vLineThresholdValue) {
        ggInt32 vPositionX = vRegionOfInterestPositionX + vIndexX;
        ggInt32 vPositionY = vRegionOfInterestPositionY + vIndexY;
        vPositionsX.push_back(vPositionX);
        vPositionsY.push_back(vPositionY);
        vAveragesX.AddSample(vPositionX);
        vAveragesY.AddSample(vPositionY);
      }
    }
  }

  // find the line
  double vC0, vC1, vCov00, vCov01, vCov02, vSumSQ;
  gsl_fit_linear(&vPositionsX[0], 1, &vPositionsY[0], 1, vPositionsX.size(),
                 &vC0, &vC1, &vCov00, &vCov01, &vCov02, &vSumSQ);

  // stop the timer
  ggFloat vCalculationTimeMicroSeconds = 0.001f * vTimer.nsecsElapsed();

  // display the results
  mOverlayPixmap = QPixmap(mImageCamera.GetSizeX(), mImageCamera.GetSizeY());
  mOverlayPixmap.fill(Qt::transparent);

  QPainter vPainter(&mOverlayPixmap);
  vPainter.setRenderHint(QPainter::Antialiasing, true);
  vPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  QPen vPen;

  // draw the region of interest
  vPen.setWidthF(1.0f);
  vPen.setColor(QColor(255, 255, 0));
  vPainter.setPen(vPen);
  vPainter.drawRect(vRegionOfInterestPositionX,
                    vRegionOfInterestPositionY,
                    vRegionOfInterestSizeX,
                    vRegionOfInterestSizeY);

  // draw the histogram
  ggFloat vScaleX = (ggFloat)mOverlayPixmap.width() / (ggFloat)vHistogram.size();
  ggFloat vScaleY = 0.2f * (ggFloat)mOverlayPixmap.height() / (ggFloat)vHistogramMax;
  for (ggUInt32 vValue = 0; vValue < vHistogram.size(); vValue++) {
    ggInt32 vPositionX = vValue * vScaleX;
    ggInt32 vPositionY = mOverlayPixmap.height();
    vPainter.drawLine(vPositionX, vPositionY, vPositionX, vPositionY-vScaleY*vHistogram[vValue]);
    if (vValue == vLineThresholdValue) {
      vPen.setColor(QColor(255, 0, 0));
      vPainter.setPen(vPen);
    }
  }

  // draw the thresholded pixels
  for (ggInt32 vIndexY = 0; vIndexY < vRegionOfInterestSizeY; vIndexY++) {
    for (ggInt32 vIndexX = 0; vIndexX < vRegionOfInterestSizeX; vIndexX++) {
      ggUInt16 vValue = vImageCameraROI(vIndexX, vIndexY);
      if (vValue > vLineThresholdValue) {
        ggInt32 vPositionX = vRegionOfInterestPositionX + vIndexX;
        ggInt32 vPositionY = vRegionOfInterestPositionY + vIndexY;
        vPainter.drawPoint(vPositionX, vPositionY);
      }
    }
  }

  // draw the original line
  vPen.setWidthF(1.0f + vLineThickness);
  vPen.setColor(QColor(0, 127, 255, 127));
  vPen.setCapStyle(Qt::RoundCap);
  vPainter.setPen(vPen);
  vPainter.drawLine(vLineBeginX,
                    vLineBeginY,
                    vLineEndX,
                    vLineEndY);

  // draw the fitted line
  vPen.setColor(QColor(255, 255, 0));
  vPen.setWidthF(1.0f);
  vPen.setStyle(Qt::DashLine);
  vPainter.setPen(vPen);
  ggFloat vX0 = 0.0f;
  ggFloat vY0 = vC0 + vC1 * vX0;
  ggFloat vX1 = mOverlayPixmap.width();
  ggFloat vY1 = vC0 + vC1 * vX1;
  vPainter.drawLine(QPointF(vX0, vY0), QPointF(vX1, vY1));

  // draw the center of gravity + stddev
  vPen.setColor(QColor(0, 255, 0));
  vPen.setWidthF(1.0f);
  vPen.setStyle(Qt::SolidLine);
  vPainter.setPen(vPen);
  QPointF vCenter(vAveragesX.GetMean(), vAveragesY.GetMean());
  ggFloat vRadiusX = vAveragesX.GetStdDev();
  ggFloat vRadiusY = vAveragesY.GetStdDev();
  vPainter.drawLine(vCenter.x(), vCenter.y()-vRadiusY, vCenter.x(), vCenter.y()-5);
  vPainter.drawLine(vCenter.x(), vCenter.y()+vRadiusY, vCenter.x(), vCenter.y()+5);
  vPainter.drawLine(vCenter.x()-vRadiusX, vCenter.y(), vCenter.x()-5, vCenter.y());
  vPainter.drawLine(vCenter.x()+vRadiusX, vCenter.y(), vCenter.x()+5, vCenter.y());
  vPen.setWidthF(0.5f);
  vPainter.setPen(Qt::NoPen);
  vPainter.setBrush(QBrush(QColor(0, 255, 0, 50)));
  vPainter.drawEllipse(vCenter, vRadiusX, vRadiusY);

  // compute distance between center of gravity and line(s)
  ggFloat vLaserLineDeltaX = vLineEndX - vLineBeginX;
  ggFloat vLaserLineDeltaY = vLineEndY - vLineBeginY;
  ggFloat vA1 = vLaserLineDeltaY / vLaserLineDeltaX;
  ggFloat vA0 = vLineBeginY - vA1 * vLineBeginX;
  ggFloat vDistanceToLaserLine = fabs(vA1*vCenter.x() - 1.0f*vCenter.y() + vA0) / sqrt(vA1*vA1 + 1.0f*1.0f);
  ggFloat vDistancetoFittedLine = fabs(vC1*vCenter.x() - 1.0f*vCenter.y() + vC0) / sqrt(vC1*vC1 + 1.0f*1.0f);

  // put some text into results box
  vUI->mResultsPlainTextEdit->setPlainText("Calculation Time = " + QString::number(vCalculationTimeMicroSeconds) + " us\n" +
                                           "===========================\n" +
                                           "Number of Points = " + QString::number(vAveragesX.GetNumberOfSamples()) + "\n" +
                                           "Mean X = " + QString::number(vAveragesX.GetMean()) + " px\n" +
                                           "Mean Y = " + QString::number(vAveragesY.GetMean()) + " px\n" +
                                           "StdDev X = " + QString::number(vAveragesX.GetStdDev()) + " px\n" +
                                           "StdDev Y = " + QString::number(vAveragesY.GetStdDev()) + " px\n" +
                                           "===========================\n" +
                                           "Line C0 = " + QString::number(vC0) + " px\n" +
                                           "Line C1 = " + QString::number(vC1) + "\n" +
                                           "Line Cov00 = " + QString::number(vCov00) + "\n" +
                                           "Line Cov01 = " + QString::number(vCov01) + "\n" +
                                           "Line Cov02 = " + QString::number(vCov02) + "\n" +
                                           "===========================\n" +
                                           "Distance CG to fitted Line = " + QString::number(vDistancetoFittedLine) + " px\n"+
                                           "Distance CG to Laser Line = " + QString::number(vDistanceToLaserLine) + " px");

  repaint();
}


void DrawCrossHair(QPainter& aPainter,
                   const QPointF& aCenter,
                   qreal aRadiusOuter,
                   qreal aRadiusInner = 0.0,
                   qreal aAngleDeg = 0.0)
{
  const QPointF vCenter = aCenter + QPointF(0.5, 0.5);
  if (aRadiusInner == 0.0) {
    qreal vAngle = aAngleDeg * M_PI / 180.0;
    QPointF vPO0(aRadiusOuter * cos(vAngle), aRadiusOuter * sin(vAngle));
    QPointF vPO1(-vPO0.y(), vPO0.x());
    aPainter.drawLine(vCenter - vPO0, vCenter + vPO0);
    aPainter.drawLine(vCenter - vPO1, vCenter + vPO1);
  }
  else {
    qreal vAngle = aAngleDeg * M_PI / 180.0;
    QPointF vPI0(aRadiusInner * cos(vAngle), aRadiusInner * sin(vAngle));
    QPointF vPO0(aRadiusOuter * cos(vAngle), aRadiusOuter * sin(vAngle));
    QPointF vPI1(-vPI0.y(), vPI0.x());
    QPointF vPO1(-vPO0.y(), vPO0.x());
    aPainter.drawLine(vCenter - vPO0, vCenter - vPI0);
    aPainter.drawLine(vCenter + vPI0, vCenter + vPO0);
    aPainter.drawLine(vCenter - vPO1, vCenter - vPI1);
    aPainter.drawLine(vCenter + vPI1, vCenter + vPO1);
  }
}


void DrawCircle(QPainter& aPainter,
                const QPointF& aCenter,
                qreal aRadius)
{
  aPainter.drawEllipse(aCenter + QPointF(0.5, 0.5), aRadius, aRadius);
}


void ggRenderWidget::ggRenderWidget::FindCircle()
{
  MainWindow* vMainWindow = dynamic_cast<MainWindow*>(aParent()->aParent());
  Ui::MainWindow* vUI = vMainWindow->GetUI();

  // read parameters from GUI
  qDebug() << "read parameters from GUI";
  const ggVector2Float vCircleCenter(vUI->mCircleCenterXLineEdit->text().toFloat(),
                                     vUI->mCircleCenterYLineEdit->text().toFloat());
  const ggFloat vCircleDiameter = vUI->mCircleDiameterLineEdit->text().toFloat();
  const ggFloat vCircleThickness = vUI->mLineThicknessLineEdit->text().toFloat();
  const ggVector2Int32 vRegionOfInterestPosition(vUI->mRegionOfInterestPositionXLineEdit->text().toInt(),
                                                 vUI->mRegionOfInterestPositionYLineEdit->text().toInt());
  const ggVector2Int32 vRegionOfInterestSize(vUI->mRegionOfInterestSizeXLineEdit->text().toInt(),
                                             vUI->mRegionOfInterestSizeYLineEdit->text().toInt());
  const ggFloat vLineModelThickness = vUI->mLineModelThicknessLineEdit->text().toFloat();
  const bool vLineModelGaussianFilter = vUI->mLineModelGaussianFilterCheckBox->isChecked();
  const ggFloat vLineModelGaussianFilterWidth = vUI->mLineModelGaussianFilterLineEdit->text().toFloat();

  qDebug() << "vCircleCenter" << ggUtilities::ToString(vCircleCenter).c_str();

  // original circle
  QPointF vOriginalCircleCenter(vCircleCenter.X(), vCircleCenter.Y());
  ggFloat vOriginalCircleRadius(vCircleDiameter / 2.0f);

  // copy image & smooth (remove the noise)
  qDebug() << "copy image & smooth (remove the noise)";
  ggImageT<ggFloat> vImageCameraROI(vRegionOfInterestSize.X(), vRegionOfInterestSize.Y());
  ggImageFilter::Copy(vImageCameraROI, mImageCamera, vRegionOfInterestPosition.X(), vRegionOfInterestPosition.Y());
  if (vLineModelGaussianFilter) ggImageFilter::Gauss(vImageCameraROI, vLineModelGaussianFilterWidth);

  // calculate gradient vector field
  qDebug() << "calculate gradient vector field";
  ggImageT<ggVector2Float> vGradientVectorField(vImageCameraROI.GetSizeX(), vImageCameraROI.GetSizeY());
  ggImageFilter::Gradient(vGradientVectorField, vImageCameraROI);

  // do a gradient based hough transformation
  qDebug() << "do a gradient based hough transformation";
  ggImageT<ggFloat> vImageHough(vImageCameraROI.GetSize(), 0.0f);
  ggImagePainterT<ggFloat> vPainterHough(vImageHough);
  for (ggSize vIndexY = 1; vIndexY+1 < vGradientVectorField.GetSizeY(); vIndexY++) {
    for (ggSize vIndexX = 1; vIndexX+1 < vGradientVectorField.GetSizeX(); vIndexX++) {
      const ggVector2Double& vGradient(vGradientVectorField(vIndexX, vIndexY).GetConverted<ggDouble>());
      if (vGradient.Length() > 0.0f) {

        // try to fibure out some sort of circle curvature depending on the neighbor gradients.
        // - triangulate a virtual center-point (accuracy might be a problem due to the close
        //   proximity of the two triangulation points). nevertheless the virtual center can be used
        //   as indicator, on which side to expect the circle center
        // - the two triangulation points are selected perpendicular to the actual gradient
        bool vPositive = vGradient.X() * vGradient.Y() > 0.0f;
        // position of two (intersecting) lines
        ggVector2Int32 vIndexA(vPositive ? vIndexX-1 : vIndexX+1, vPositive ? vIndexY+1 : vIndexY+1);
        ggVector2Int32 vIndexB(vPositive ? vIndexX+1 : vIndexX-1, vPositive ? vIndexY-1 : vIndexY-1);
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
          ggDouble vRadius = vCenterDirection.Dot(vGradient) > 0.0f ? vOriginalCircleRadius : -vOriginalCircleRadius;
          // use the actual gradient (and not the gradient from neighbors) for the center voting
          vCenterDirection = vRadius * vGradient.Normalized();
          // the position of the center point may varies by the thickenss of the circle line
          ggVector2Double vRadiusRange(0.5f * vLineModelThickness * vGradient.Normalized());
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
  qDebug() << "smooth hough image";
  if (vLineModelGaussianFilter) ggImageFilter::Gauss(vImageHough, vLineModelGaussianFilterWidth);

  // find all local maxima
  qDebug() << "find all local maxima";
  typedef ggSpotT<ggFloat, ggVector2Double> tSpot;
  typedef std::vector<tSpot> tSpots;
  tSpots vLocalMaxima = ggImageFilter::FindLocalMaxima(vImageHough, true);

  // sort local maxima from highest to lowest
  qDebug() << "sort local maxima from highest to lowest";
  std::sort(vLocalMaxima.begin(), vLocalMaxima.end(),
            [] (const tSpot& aPA, const tSpot& aPB) {return aPA.GetValue() > aPB.GetValue();} );

  // convert hough image for rendering with QT
  qDebug() << "convert hough image for rendering with QT";
  ggFloat vMin, vMax;
  ggImageFilter::GetMinMax(vImageHough, vMin, vMax);
  ggImageT<ggUChar> vImageUChar(vImageHough.GetSizeX(), vImageHough.GetSizeY());
  for (ggSize vIndexY = 0; vIndexY < vImageHough.GetSizeY(); vIndexY++) {
    for (ggSize vIndexX = 0; vIndexX < vImageHough.GetSizeX(); vIndexX++) {
      float vValue = (vImageHough(vIndexX, vIndexY) - vMin) / (vMax - vMin);
      vImageUChar(vIndexX, vIndexY) = 255.0f * vValue;
    }
  }

  QImage vImage(vImageUChar.GetValues(),
                vImageUChar.GetSizeX(),
                vImageUChar.GetSizeY(),
                vImageUChar.GetSizeX(),
                QImage::Format_Indexed8);

  // generate a nice color table for the hough image
  qDebug() << "generate a nice color table for the hough image";
  QVector<QRgb> vColorTable(256);
  for (int vIndex = 0; vIndex < vColorTable.size(); vIndex++) {
    if      (vIndex <  32) vColorTable[vIndex] = QColor(2*vIndex,          0,             8*vIndex      ).rgb();
    else if (vIndex <  64) vColorTable[vIndex] = QColor(2*vIndex,          0,             255           ).rgb();
    else if (vIndex <  96) vColorTable[vIndex] = QColor(3*(vIndex-64)+128, 0,             4*(127-vIndex)).rgb();
    else if (vIndex < 128) vColorTable[vIndex] = QColor(1*(vIndex-96)+224, 2*(vIndex-96), 4*(127-vIndex)).rgb();
    else if (vIndex < 160) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 0             ).rgb();
    else if (vIndex < 192) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 0             ).rgb();
    else if (vIndex < 224) vColorTable[vIndex] = QColor(255,               2*(vIndex-96), 4*(vIndex-192)).rgb();
    else                   vColorTable[vIndex] = QColor(255,               255,           4*(vIndex-192)).rgb();
    // for (int vX = 0; vX < 50; vX++) vImageUChar(vX, vIndex) = vIndex;
  }
  vImage.setColorTable(vColorTable);

  // display the results
  mOverlayPixmap = QPixmap(mImageCamera.GetSizeX(), mImageCamera.GetSizeY());
  mOverlayPixmap.fill(Qt::transparent);

  QPainter vPainter(&mOverlayPixmap);
  vPainter.setRenderHint(QPainter::Antialiasing, true);
  vPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  vPainter.drawImage(vRegionOfInterestPosition.X(),
                     vRegionOfInterestPosition.Y(),
                     vImage);

  QPen vPen;

  // draw the region of interest
  vPen.setWidthF(1.0f);
  vPen.setColor(QColor(255, 255, 0));
  vPainter.setPen(vPen);
  vPainter.drawRect(vRegionOfInterestPosition.X(),
                    vRegionOfInterestPosition.Y(),
                    vRegionOfInterestSize.X(),
                    vRegionOfInterestSize.Y());

  // draw original circle
  vPen.setWidthF(1.0 + vCircleThickness);
  vPen.setColor(QColor(0, 255, 150, 100));
  vPen.setCapStyle(Qt::RoundCap);
  vPainter.setPen(vPen);
  DrawCircle(vPainter, vOriginalCircleCenter, vOriginalCircleRadius);
  vPen.setWidthF(3.0);
  vPen.setColor(Qt::black);
  vPainter.setPen(vPen);
  DrawCrossHair(vPainter, vOriginalCircleCenter, 10.0, 4.0);
  vPen.setWidthF(1.5);
  vPen.setColor(Qt::white);
  vPainter.setPen(vPen);
  DrawCrossHair(vPainter, vOriginalCircleCenter, 10.0, 4.0);

  // draw the local maxima
  QString vResults;
  ggInt32 vIndexMax = std::min<ggInt32>(vLocalMaxima.size() - 1, 9);
  ggFloat vSpotValueMax = vLocalMaxima.empty() ? 0.0f : vLocalMaxima.front().GetValue();
  for (ggInt32 vIndex = vIndexMax; vIndex >= 0; vIndex--) {
    const tSpot& vLocalMaximum = vLocalMaxima[vIndex];
    QPointF vLocalMaximumPosition(vRegionOfInterestPosition.X() + vLocalMaximum.X(),
                                  vRegionOfInterestPosition.Y() + vLocalMaximum.Y());
    vPainter.setPen(QPen(Qt::black, 3.0));
    DrawCrossHair(vPainter, vLocalMaximumPosition, 10.0, 4.0, 45.0);
    vPainter.setPen(QPen(QColor(vIndex == 0 ? 0 : 255, (int)(255.0f * vLocalMaximum.GetValue() / vSpotValueMax + 0.5f), 0), 1.5));
    DrawCrossHair(vPainter, vLocalMaximumPosition, 10.0, 4.0, 45.0);
    if (vIndex < 5) {
      DrawCircle(vPainter, vLocalMaximumPosition, vOriginalCircleRadius);
      // vResults += "Local Maximum [" + QString::number(vIndex) + "] => " +  + "\n";
      qDebug() << "local max =" << vLocalMaximumPosition << "value =" << vLocalMaximum.GetValue();
    }
  }

  /*
  // find THE circle
  if (!vLocalMaxima.empty()) {
    vPainter.setPen(QPen(QColor(255, 255, 255, 100), 1));

    const ggInt32 vRange = vOriginalCircleRadius;
    const tSpot& vLocalMaximum = vLocalMaxima.front();
    const ggInt32 vMinX = std::max<ggInt32>(vLocalMaximum.GetX() + 0.5 - vRange, 0);
    const ggInt32 vMinY = std::max<ggInt32>(vLocalMaximum.GetY() + 0.5 - vRange, 0);
    const ggInt32 vMaxX = std::min<ggInt32>(vLocalMaximum.GetX() + 0.5 + vRange, vImageHough.GetSizeX());
    const ggInt32 vMaxY = std::min<ggInt32>(vLocalMaximum.GetY() + 0.5 + vRange, vImageHough.GetSizeY());
    ggVector2T<ggRunningAveragesT<ggDouble> > vCircleCenterHoughAverage;
    for (ggInt32 vIndexY = vMinY; vIndexY < vMaxY; vIndexY++) {
      for (ggInt32 vIndexX = vMinX; vIndexX < vMaxX; vIndexX++) {
        const ggFloat& vImageHoughValue = vImageHough(vIndexX, vIndexY);
        const ggFloat vThresholdValue = vLocalMaximum.GetValue() / 2.0;
        if (vImageHoughValue > vThresholdValue) {
          vCircleCenterHoughAverage.GetX().AddSample(vRegionOfInterestPositionX + vIndexX, vImageHoughValue - vThresholdValue);
          vCircleCenterHoughAverage.GetY().AddSample(vRegionOfInterestPositionY + vIndexY, vImageHoughValue - vThresholdValue);
          vPainter.drawPoint(vRegionOfInterestPositionX + vIndexX, vRegionOfInterestPositionY + vIndexY);
        }
      }
    }

    QPointF vCircleCenterFinal(vCircleCenterHoughAverage.GetX().GetMean(),
                               vCircleCenterHoughAverage.GetY().GetMean());

    vPainter.setPen(QPen(Qt::black, 3.0));
    DrawCrossHair(vPainter, vCircleCenterFinal, 10.0, 4.0, 45.0);
    vPainter.setPen(QPen(QColor(0, 255, 0), 1.5));
    DrawCrossHair(vPainter, vCircleCenterFinal, 10.0, 4.0, 45.0);
    DrawCircle(vPainter, vCircleCenterFinal, vOriginalCircleRadius);
    qDebug() << "center =" << vCircleCenterFinal;
    qDebug() << "stddev x =" << vCircleCenterHoughAverage.GetX().GetStdDev();
    qDebug() << "stddev y =" << vCircleCenterHoughAverage.GetY().GetStdDev();
  }
  */
  /*
  // draw histograms
  ggHistogramFloat vHistogramROI = ggImageFilter::GetHistogram2(vImageCameraROI);
  ggInt64 vBinIndexMin = vHistogramROI.GetBinIndexMin();
  ggInt64 vBinIndexMax = vHistogramROI.GetBinIndexMax();
  ggFloat vCountMaxF = 0.0f;
  for (ggInt64 vBinIndex = vBinIndexMin; vBinIndex <= vBinIndexMax; vBinIndex++) {
    ggInt64 vCount = vHistogramROI.GetCountBin(vBinIndex);
    if (vCountMaxF < vCount) vCountMaxF = vCount;
  }
  for (ggInt64 vBinIndex = vBinIndexMin; vBinIndex <= vBinIndexMax; vBinIndex++) {
    ggInt64 vPosX = vBinIndex - vBinIndexMin + 10;
    ggInt64 vPosY = vPainter.device()->height() - 20;
    ggFloat vCountF = vHistogramROI.GetCountBin(vBinIndex);
    vPainter.drawLine(vPosX, vPosY, vPosX, vPosY - 200 * log(vCountF/vCountMaxF + 1.0));
  }

  vPen.setColor(QColor(255, 0, 0));
  vPainter.setPen(vPen);
  ggHistogramFloat vHistogramHough = ggImageFilter::GetHistogram2(vImageHough);
  vBinIndexMin = vHistogramHough.GetBinIndexMin();
  vBinIndexMax = vHistogramHough.GetBinIndexMax();
  vCountMaxF = 0.0f;
  for (ggInt64 vBinIndex = vBinIndexMin; vBinIndex <= vBinIndexMax; vBinIndex++) {
    ggInt64 vCount = vHistogramHough.GetCountBin(vBinIndex);
    if (vCountMaxF < vCount) vCountMaxF = vCount;
  }
  for (ggInt64 vBinIndex = vBinIndexMin; vBinIndex <= vBinIndexMax; vBinIndex++) {
    ggInt64 vPosX = vBinIndex - vBinIndexMin + 10;
    ggInt64 vPosY = vPainter.device()->height() - 10;
    ggFloat vCountF = vHistogramHough.GetCountBin(vBinIndex);
    vPainter.drawLine(vPosX, vPosY, vPosX, vPosY - 200 * log(vCountF/vCountMaxF + 1.0));
  }
  */

  // put some text into results box
  vUI->mResultsPlainTextEdit->setPlainText(vResults);

  // repaint...
  repaint();
}


void ggRenderWidget::paintEvent(QPaintEvent* /* event */)
{
  MainWindow* vMainWindow = dynamic_cast<MainWindow*>(aParent()->aParent());
  Ui::MainWindow* vUI = vMainWindow->GetUI();

  const ggInt32 vOpacitySliderValue = vUI->mOverlayOpacitySlider->value();
  const ggInt32 vOpacitySliderValueMin = vUI->mOverlayOpacitySlider->minimum();
  const ggInt32 vOpacitySliderValueMax = vUI->mOverlayOpacitySlider->maximum();

  QPainter vPainter(this);

  vPainter.setRenderHint(QPainter::Antialiasing, true);
  vPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  vPainter.translate(width()/2, height()/2);

  float vScaleX = (float)width() / (float)mImagePixmap.width();
  float vScaleY = (float)height() / (float)mImagePixmap.height();
  float vScale = std::min(vScaleX, vScaleY);

  //if (vScale < 1.0f) {
    vPainter.scale(vScale, vScale);
  //}

  vPainter.drawPixmap(-mImagePixmap.width() / 2,
                      -mImagePixmap.height() / 2,
                      mImagePixmap);

  const ggFloat vOpacity = (ggFloat)(vOpacitySliderValue - vOpacitySliderValueMin) /
                           (ggFloat)(vOpacitySliderValueMax - vOpacitySliderValueMin);

  QPainter::PixmapFragment vFragment = QPainter::PixmapFragment::create(QPointF(0.0, 0.0), mOverlayPixmap.rect(), 1.0, 1.0, 0.0, vOpacity);

  vPainter.drawPixmapFragments(&vFragment, 1, mOverlayPixmap);

}
