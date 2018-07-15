#ifndef GGGRAPHICSSCENE_H
#define GGGRAPHICSSCENE_H

#include <QDebug>
#include <QGraphicsScene>

#include "LibBase/ggGeometry.h"
#include "LibImage/ggImageT.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggImageAlgorithm.h"
#include "QtBase/ggUtilityQt.h"
#include "QtGraphics/ggGraphicsManipulatorRectItemT.h"


class ggGraphicsScene :
  public QGraphicsScene
{
public:

  typedef ggGraphicsManipulatorRectItemT<> tROI;

  ggGraphicsScene(QObject* aParent)
  : QGraphicsScene(aParent) {

    mImageCameraPixmapItem = new QGraphicsPixmapItem();
    mImageCameraPixmapItem->setZValue(0.0);
    addItem(mImageCameraPixmapItem);

    mImageHoughPixmapItem = new QGraphicsPixmapItem();
    mImageHoughPixmapItem->setZValue(1.0);
    addItem(mImageHoughPixmapItem);

    mROI = new tROI(QRectF(10,10,100,100));
    mROI->setPen(QPen(QColor(255, 200, 0), 0.0));
    mROI->SetHandleColor(QColor(255, 200, 0));
    mROI->setZValue(3.0);
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
    vImageFloat.GetMinMax(vMin, vMax);
    ggImageFilter::AddNoise(vImageFloat, aCameraNoise * (vMin != vMax ?  vMax - vMin : 1.0f));
    vImageFloat.GetMinMax(vMin, vMax);

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

  QString FindCircles(const bool aCircleModelGaussianFilter,
                      const ggFloat aCircleModelGaussianFilterWidth,
                      const ggFloat aCircleModelDiameter,
                      const ggFloat aCircleModelLineThickness,
                      const bool aCircleModelCenterVotesFilter,
                      const ggFloat aCircleModelCenterVotesFilterWidth,
                      const ggInt32 aCircleModelNumberOfCircles)
  {
    // copy ROI image & smooth (remove the noise)
    qDebug() << "copy ROI image & smooth (remove the noise)";
    ggImageT<ggFloat> vImageCameraROI(GetROISize().width(), GetROISize().height(), 0.0f);
    mImageCamera.Copy(vImageCameraROI, GetROIPosition().x(), GetROIPosition().y());
    if (aCircleModelGaussianFilter) ggImageFilter::Gauss(vImageCameraROI, aCircleModelGaussianFilterWidth);

    // do gradient based hough transformation
    qDebug() << "do gradient based hough transformation";
    ggImageT<ggFloat> vImageHough(ggImageAlgorithm::ComputeHoughImage(vImageCameraROI,
                                                                      aCircleModelDiameter,
                                                                      aCircleModelLineThickness));

    // smooth hough image
    qDebug() << "smooth hough voting image";
    if (aCircleModelCenterVotesFilter) ggImageFilter::Gauss(vImageHough, aCircleModelCenterVotesFilterWidth);

    // detect center spots by finding all local maxima
    qDebug() << "detect center spots by finding all local maxima";
    typedef ggSpotT<ggFloat, ggVector2Double> tSpot;
    typedef std::vector<tSpot> tSpots;
    tSpots vCenterSpots = ggImageAlgorithm::FindLocalMaxima(vImageHough, true);

    // convert hough image for rendering with QT
    qDebug() << "convert hough image for rendering with QT";
    ggImageT<ggUChar> vImageUChar = vImageHough.GetConvertedFitMinMax<ggUChar>();
    std::vector<ggColorUInt8> vColorTableUInt8 = ggUtility::ColorTable();
    QImage vImageQt = ggUtilityQt::GetImage(vImageUChar, vColorTableUInt8);
    mImageHoughPixmapItem->setPos(GetROIPosition());
    mImageHoughPixmapItem->setPixmap(QPixmap::fromImage(vImageQt));

    // draw the detected circles
    std::for_each(mCircleItems.begin(), mCircleItems.end(), [] (QGraphicsEllipseItem* aItem) { delete aItem; });
    mCircleItems.clear();
    QString vResults = "Found " + QString::number(vCenterSpots.size()) + " Candidates\n";
    ggInt32 vCenterIndexMax = std::min<ggInt32>(vCenterSpots.size() - 1, aCircleModelNumberOfCircles - 1);
    ggFloat vCenterSpotValueMax = vCenterSpots.empty() ? 0.0f : vCenterSpots.front().GetValue();
    for (ggInt32 vSpotIndex = vCenterIndexMax; vSpotIndex >= 0; vSpotIndex--) {
      const tSpot& vCenterSpot = vCenterSpots[vSpotIndex];
      const ggVector2Double vCircleCenter = vCenterSpot.GetConverted<ggDouble>() + ggVector2Double(GetROIPosition().x(), GetROIPosition().y());
      QPointF vCenterSpotPointF(ggUtilityQt::GetPointF(vCircleCenter));
      /*
      vPainter.setPen(QPen(Qt::black, 3.0));
      DrawCrossHair(vPainter, vCenterSpotPointF, 10.0, 4.0, 45.0);
      vPainter.setPen(QPen(QColor(vSpotIndex == 0 ? 0 : 255, (int)(255.0f * vCenterSpot.GetValue() / vCenterSpotValueMax + 0.5f), 0), 1.5));
      DrawCrossHair(vPainter, vCenterSpotPointF, 10.0, 4.0, 45.0);
      */
      QPen vPen(QColor(vSpotIndex == 0 ? 0 : 255, (int)(255.0f * vCenterSpot.GetValue() / vCenterSpotValueMax + 0.5f), 0, 255), aCircleModelLineThickness);
      AddCircle(vCenterSpotPointF, aCircleModelDiameter / 2.0, vPen);
      vResults += QString::number(vSpotIndex) + ": Pos" + ggUtility::ToString(vCircleCenter, 2).c_str() + " Val(" + ggUtility::ToString(ggUtility::RoundToSD(vCenterSpot.GetValue())).c_str() + ")\n";
    }

    return vResults;
  }

  void SetHoughImageOpacity(ggFloat aOpacity)
  {
    mImageHoughPixmapItem->setOpacity(aOpacity);
  }

  void SetCirclesOpacity(ggFloat aOpacity)
  {
    std::for_each(mCircleItems.begin(), mCircleItems.end(), [aOpacity] (QGraphicsItem* aItem) {
      aItem->setOpacity(aOpacity);
    });
  }

private:

  /*
  void AddCrossHair(QPainter& aPainter,
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
  */

  void AddCircle(const QPointF& aCenter,
                 qreal aRadius,
                 const QPen& aPen)
  {
    QGraphicsEllipseItem* vCircle = addEllipse(aCenter.x() - aRadius + 0.5,
                                               aCenter.y() - aRadius + 0.5,
                                               2.0 * aRadius,
                                               2.0 * aRadius);
    vCircle->setZValue(2.0);
    vCircle->setPen(aPen);
    mCircleItems.push_back(vCircle);
  }

  tROI* mROI;

  ggImageT<ggUInt16> mImageCamera;
  QGraphicsPixmapItem* mImageCameraPixmapItem;

  QGraphicsPixmapItem* mImageHoughPixmapItem;

  std::vector<QGraphicsEllipseItem*> mCircleItems;

};


#endif // GGGRAPHICSSCENE_H
