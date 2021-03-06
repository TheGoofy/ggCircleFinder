#ifndef GGGRAPHICSSCENE_H
#define GGGRAPHICSSCENE_H

#include <QDebug>
#include <QGraphicsScene>
#include <QElapsedTimer>
#include <QGraphicsLineItem>
#include <QLineF>

#define _USE_MATH_DEFINES
#include <math.h>

#include "LibBase/ggGeometry.h"
#include "LibBase/ggFilterT.h"
#include "LibBase/ggRound.h"
#include "LibImage/ggImageT.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggImageAlgorithm.h"
#include "LibImage/ggSegmentation.h"
#include "LibImage/ggImageLabeling.h"
#include "LibImage/ggImageTest.h"
#include "QtBase/ggUtilityQt.h"
#include "QtGraphics/ggGraphicsHistogramRectItem.h"
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
                          ggUInt8 aCameraNumberOfBits)
  {
    ggImageT<ggFloat> vImageFloat(aImageFloat);

    /*
    ggImageAlgorithm::Invert(vImageFloat);
    ggImageFilter::SubtractBackground(vImageFloat, 100.0);
    ggImageFilter::Median(vImageFloat, 4);
    */

    ggFloat vMin = 0.0f;
    ggFloat vMax = 0.0f;
    vImageFloat.GetMinMax(vMin, vMax);
    ggImageFilter::AddNoise(vImageFloat, aCameraNoise * (vMin < vMax ?  vMax - vMin : 1.0f));
    vImageFloat.GetMinMax(vMin, vMax);

    ggUInt16 vCameraValueMax = (static_cast<ggUInt16>(1 << aCameraNumberOfBits) - 1);
    ggFloat vScaleCameraToDisplay = 255.0f / static_cast<ggFloat>(vCameraValueMax);
    mImageCamera.Resize(vImageFloat.GetSizeX(), vImageFloat.GetSizeY());
    ggImageT<ggUChar> vImageUChar(vImageFloat.GetSizeX(), vImageFloat.GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < vImageFloat.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < vImageFloat.GetSizeX(); vIndexX++) {
        float vValue = (vImageFloat(vIndexX, vIndexY) - vMin) / (vMax - vMin);
        ggUInt16 vCameraValue = static_cast<ggUInt16>(vCameraValueMax * vValue + 0.5f);
        mImageCamera(vIndexX, vIndexY) = vCameraValue;
        ggUChar vDisplayValue = static_cast<ggUChar>(vScaleCameraToDisplay * vCameraValue);
        vImageUChar(vIndexX, vIndexY) = vDisplayValue;
      }
    }

    QImage vImage(vImageUChar.GetValues(),
                  static_cast<int>(vImageUChar.GetSizeX()),
                  static_cast<int>(vImageUChar.GetSizeY()),
                  static_cast<int>(vImageUChar.GetSizeX()),
                  QImage::Format_Grayscale8);

    mImageCameraPixmapItem->setPixmap(QPixmap::fromImage(vImage));

    auto vItems = mImageCameraPixmapItem->childItems();
    std::for_each(vItems.begin(), vItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
    ggGraphicsHistogramRectItem* vHistogramItem = new ggGraphicsHistogramRectItem(mImageCameraPixmapItem);
    ggHistogram* vHistogram = ggImageFilter::GetHistogram(mImageCamera).Clone();
    vHistogramItem->SetHistogram(vHistogram, true);
    vHistogramItem->SetHistogramColor(QColor(0, 100, 255));

  }

  QString FindCircles(const bool aCircleModelGaussianFilter,
                      const ggFloat aCircleModelGaussianFilterWidth,
                      const ggDouble aCircleModelDiameter,
                      const ggDouble aCircleModelLineThickness,
                      const bool aCircleModelCenterVotesFilter,
                      const ggFloat aCircleModelCenterVotesFilterWidth,
                      const ggInt32 aCircleModelNumberOfCircles)
  {
    // start a timer
    QElapsedTimer vTimer;
    vTimer.start();

    // copy ROI image & smooth (remove the noise)
    ggImageT<ggFloat> vImageCameraROI(GetROISize().width(), GetROISize().height(), 0.0f);
    mImageCamera.Copy(vImageCameraROI, GetROIPosition().x(), GetROIPosition().y());
    if (aCircleModelGaussianFilter) ggImageFilter::Gauss(vImageCameraROI, aCircleModelGaussianFilterWidth);


    {
      /*
      // ggImageFilter::SubtractBackground(vImageCameraROI, aCircleModelDiameter);
      auto vHistogram = ggImageFilter::GetHistogram(vImageCameraROI);

      ggDouble vThresholdA = 0.0;
      ggDouble vThresholdB = 0.0;
      ggSegmentation::CalculateThresholdThreeMeans(vHistogram, vThresholdA, vThresholdB);

      // ggDouble vThresholdA = 0.0;
      // ggSegmentation::CalculateThresholdTwoMeans(vHistogram, vThresholdA);

      ggFloat vThreshold = ggRound<ggFloat>(vThresholdB);
      std::function<bool (const ggFloat& aValue)> vThresholdCheck = [vThreshold] (const ggFloat& aValue) -> bool {
        return aValue < vThreshold;
      };
      */
      /*
      ggImageT<ggInt32> vLabelImage(ggImageLabeling::CalculateConnectedComponents(vImageCameraROI,
                                                                                  vThresholdCheck,
                                                                                  ggImageLabeling::cConnectivity::eCorner));

      // convert label image for rendering with QT
      ggImageT<ggUChar> vImageUChar = vLabelImage.GetProcessed<ggUChar>([] (const ggInt32& aValue) {
        return aValue > 0 ? static_cast<ggUChar>( aValue % 128) + 0 :
                            static_cast<ggUChar>(-aValue % 128) + 128;
      });
      std::vector<ggColorUInt8> vColorTableFG = ggUtility::ColorTableRandomHot();
      std::vector<ggColorUInt8> vColorTableBG = ggUtility::ColorTableRandomCold();
      std::vector<ggColorUInt8> vColorTable;
      vColorTable.insert(vColorTable.end(), vColorTableFG.begin(), vColorTableFG.begin() + 128);
      vColorTable.insert(vColorTable.end(), vColorTableBG.begin(), vColorTableBG.begin() + 128);
      */

      // std::vector<ggColorUInt8> vColorTable = ggUtility::ColorTableHot();

      /*
      ggImageT<ggInt32> vDistanceImage(ggImageLabeling::CalculateDistanceTransformCDA3(vImageCameraROI,
                                                                                       vThresholdCheck,
                                                                                       ggImageLabeling::cConnectivity::eCorner));

      ggImageT<ggUChar> vImageUChar = vDistanceImage.GetProcessed<ggUChar>([] (const ggInt32& aValue) {
        return static_cast<ggUChar>(aValue / 3);
      });
      */
      /*
      ggImageT<ggFloat> vDistanceImage(ggImageLabeling::CalculateDistanceTransform8SED(vImageCameraROI,
                                                                                       vThresholdCheck));

      ggImageT<ggUChar> vImageUChar = vDistanceImage.GetProcessed<ggUChar>([] (const ggFloat& aValue) {
        return static_cast<ggUChar>(aValue);
      });
      */
      /*
      ggImageT<ggImageLabeling::cTopologyType> vTopologyImage = ggImageLabeling::CalculateTopology(vImageCameraROI);

      vImageUChar = vTopologyImage.GetProcessed<ggUChar>([] (const ggImageLabeling::cTopologyType& aTopologyType) {
        return static_cast<ggUChar>(aTopologyType);
      });

      vColorTable[0].Set(255,   0,   0, 255); // unknown
      vColorTable[1].Set(100, 200, 100, 127); // plateau
      vColorTable[2].Set(127, 127, 127, 127); // flank
      vColorTable[3].Set(  0, 100, 255, 255); // local min
      vColorTable[4].Set(  0, 200, 255, 255); // valley
      vColorTable[5].Set(  0, 255,   0, 255); // saddle
      vColorTable[6].Set(255, 155,   0, 255); // ridge
      vColorTable[7].Set(255, 255,   0, 255); // local max
      */
      /*
      ggImageT<ggInt32> vMinMaxImage(ggImageLabeling::CalculateLocalMinMax(vImageCameraROI));

      // ggImageTest::Print(vMinMaxImage.GetConverted<ggInt8>());

      ggImageT<ggUChar> vImageUChar = vMinMaxImage.GetProcessed<ggUChar>([] (const ggInt32& aLabel) {
        return static_cast<ggUChar>(aLabel % 128);
      });

      std::vector<ggColorUInt8> vColorTableFG = ggUtility::ColorTableRandomHot();
      std::vector<ggColorUInt8> vColorTableBG = ggUtility::ColorTableRandomCold();
      std::vector<ggColorUInt8> vColorTable;
      vColorTable.insert(vColorTable.end(), vColorTableFG.begin(), vColorTableFG.begin() + 128);
      vColorTable.insert(vColorTable.end(), vColorTableBG.begin(), vColorTableBG.begin() + 128);

      vColorTable[0].Set(127, 127, 127, 127); // no min/max
      vColorTable[1].Set(255, 255, 0, 255); // single pixel max
      vColorTable[255].Set(0, 0, 255, 255); // single pixel min
      */
      /*
      // stop the timer
      ggDouble vCalculationTimeMicroSeconds = 0.001 * vTimer.nsecsElapsed();
      QString vResults = "Calculation Time = " + QString::number(vCalculationTimeMicroSeconds) + " us\n";

      QImage vImageQt = ggUtilityQt::GetImage(vImageUChar, vColorTable);
      mImageHoughPixmapItem->setPos(GetROIPosition());
      mImageHoughPixmapItem->setPixmap(QPixmap::fromImage(vImageQt));
      */
      /*
      // draw color bar(s)
      ggImageT<ggUChar> vImageColorBar(256, 10);
      vImageColorBar.ProcessIndex([&vImageColorBar] (ggSize aIndexX, ggSize aIndexY) {
        vImageColorBar(aIndexX, aIndexY) = static_cast<ggUChar>(aIndexX);
      });
      for (ggSize vColorBarIndex = 0; vColorBarIndex < 5; vColorBarIndex++) {
        std::vector<ggColorUInt8> vColorTable;
        switch (vColorBarIndex) {
          case 0: vColorTable = ggUtility::ColorTableHot(); break;
          case 1: vColorTable = ggUtility::ColorTableRainbow(); break;
          case 2: vColorTable = ggUtility::ColorTableRandomRainbow(); break;
          case 3: vColorTable = ggUtility::ColorTableRandomHot(); break;
          case 4: vColorTable = ggUtility::ColorTableRandomCold(); break;
          default: break;
        }
        QImage vImageColorBarQt = ggUtilityQt::GetImage(vImageColorBar, vColorTable);
        QGraphicsItem* vColorBarPixmapItem = addPixmap(QPixmap::fromImage(vImageColorBarQt));
        vColorBarPixmapItem->setPos(0, vColorBarIndex * vImageColorBar.GetSizeY());
      }
      */
      /*
      return "Testing:\n" + vResults;
      */
    }

    // do gradient based hough transformation
    ggImageT<ggFloat> vImageHough(ggImageAlgorithm::CalculateHoughImage(vImageCameraROI,
                                                                        aCircleModelDiameter,
                                                                        aCircleModelLineThickness));

    // smooth hough image
    if (aCircleModelCenterVotesFilter) ggImageFilter::Gauss(vImageHough, aCircleModelCenterVotesFilterWidth);

    // detect center spots by finding all local maxima
    typedef ggSpotT<ggFloat, ggVector2Double> tSpot;
    typedef std::vector<tSpot> tSpots;
    tSpots vCenterSpots = ggImageAlgorithm::FindLocalMaxima2(vImageHough, true);

    // interpolate the brightest spot centers by calculating the center of gravity
    ggUSize vNumberOfInterestingSpots = std::min<ggSize>(vCenterSpots.size(), aCircleModelNumberOfCircles);
    for (ggUSize vSpotIndex = 0; vSpotIndex < vNumberOfInterestingSpots; vSpotIndex++) {
      tSpot& vSpot = vCenterSpots[vSpotIndex];
      ggImageAlgorithm::CalculateCenterOfGravity(vImageHough,
                                                 ggRound<ggSize>(vSpot[0]), ggRound<ggSize>(vSpot[1]),
                                                 ggRound<ggSize>(std::min(4 * aCircleModelLineThickness, aCircleModelDiameter / 4)),
                                                 vSpot[0], vSpot[1]);
    }

    // stop the timer
    ggDouble vCalculationTimeMicroSeconds = 0.001 * vTimer.nsecsElapsed();
    QString vResults = "Calculation Time = " + QString::number(vCalculationTimeMicroSeconds) + " us\n";

    // convert hough image for rendering with QT
    ggImageT<ggUChar> vImageUChar = vImageHough.GetConvertedFitMinMax<ggUChar>();
    std::vector<ggColorUInt8> vColorTableUInt8 = ggUtility::ColorTableHot();
    QImage vImageQt = ggUtilityQt::GetImage(vImageUChar, vColorTableUInt8);
    mImageHoughPixmapItem->setPos(GetROIPosition());
    mImageHoughPixmapItem->setPixmap(QPixmap::fromImage(vImageQt));

    auto vItems = mImageHoughPixmapItem->childItems();
    std::for_each(vItems.begin(), vItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
    ggGraphicsHistogramRectItem* vHistogramItem = new ggGraphicsHistogramRectItem(mImageHoughPixmapItem);
    ggHistogram* vHistogram = ggImageFilter::GetHistogram(vImageHough).Clone();
    vHistogramItem->SetHistogramColor(QColor(255, 0, 100));
    vHistogramItem->SetHistogram(vHistogram, true);

    // draw the detected circles
    std::for_each(mCircleItems.begin(), mCircleItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
    mCircleItems.clear();
    vResults += "Found " + QString::number(vCenterSpots.size()) + " Candidates\n";
    ggInt32 vCenterIndexMax = std::min<ggInt32>(static_cast<ggInt32>(vCenterSpots.size()) - 1, aCircleModelNumberOfCircles - 1);
    ggFloat vCenterSpotValueMax = vCenterSpots.empty() ? 0.0f : vCenterSpots.front().GetValue();
    for (ggInt32 vSpotIndex = vCenterIndexMax; vSpotIndex >= 0; vSpotIndex--) {
      const tSpot& vCenterSpot = vCenterSpots[static_cast<ggUSize>(vSpotIndex)];
      const ggVector2Double vCircleCenter = vCenterSpot.GetConverted<ggDouble>() + ggVector2Double(GetROIPosition().x(), GetROIPosition().y());
      QPointF vCenterSpotPointF(ggUtilityQt::GetPointF(vCircleCenter));
      QColor vColor = QColor::fromRgbF(vSpotIndex == 0 ? 0.0 : 1.0,
                                       static_cast<qreal>(vCenterSpot.GetValue() / vCenterSpotValueMax),
                                       0.0);
      QPen vPen(vColor, static_cast<qreal>(aCircleModelLineThickness));
      AddCircle(vCenterSpotPointF, static_cast<qreal>(aCircleModelDiameter) / 2.0, static_cast<qreal>(vCenterSpot.GetValue()), vPen);
      vResults += QString::number(vSpotIndex) + " : " + ggUtility::ToString(vCircleCenter, 2).c_str() + " : " +
                  ggUtility::ToString(ggRoundToSD(vCenterSpot.GetValue(), 3)).c_str() + "\n";
    }
    SetCirclesZValue(2.0);

    return vResults;
  }

  void SetHoughImageOpacity(qreal aOpacity)
  {
    mImageHoughPixmapItem->setOpacity(aOpacity);
  }

  void SetCirclesOpacity(qreal aOpacity)
  {
    std::for_each(mCircleItems.begin(), mCircleItems.end(), [aOpacity] (QGraphicsItem* aItem) {
      aItem->setOpacity(aOpacity);
    });
  }

private:

  void SetCirclesZValue(qreal aZValue)
  {
    std::for_each(mCircleItems.begin(), mCircleItems.end(), [aZValue] (QGraphicsItem* aItem) {
      aItem->setZValue(aZValue);
    });
  }

  void AddCrossHair(const QPointF& aCenter,
                    qreal aRadiusOuter,
                    qreal aRadiusInner,
                    qreal aAngleDeg,
                    const QPen& aPen)
  {
    const QPointF vCenter = aCenter + QPointF(0.5, 0.5);
    if (aRadiusInner == 0.0) {
      qreal vAngle = aAngleDeg * M_PI / 180.0;
      QPointF vPO0(aRadiusOuter * cos(vAngle), aRadiusOuter * sin(vAngle));
      QPointF vPO1(-vPO0.y(), vPO0.x());
      mCircleItems.push_back(addLine(QLineF(vCenter - vPO0, vCenter + vPO0), aPen));
      mCircleItems.push_back(addLine(QLineF(vCenter - vPO1, vCenter + vPO1), aPen));
    }
    else {
      qreal vAngle = aAngleDeg * M_PI / 180.0;
      QPointF vPI0(aRadiusInner * cos(vAngle), aRadiusInner * sin(vAngle));
      QPointF vPO0(aRadiusOuter * cos(vAngle), aRadiusOuter * sin(vAngle));
      QPointF vPI1(-vPI0.y(), vPI0.x());
      QPointF vPO1(-vPO0.y(), vPO0.x());
      mCircleItems.push_back(addLine(QLineF(vCenter - vPO0, vCenter - vPI0), aPen));
      mCircleItems.push_back(addLine(QLineF(vCenter + vPI0, vCenter + vPO0), aPen));
      mCircleItems.push_back(addLine(QLineF(vCenter - vPO1, vCenter - vPI1), aPen));
      mCircleItems.push_back(addLine(QLineF(vCenter + vPI1, vCenter + vPO1), aPen));
    }
  }

  void AddCircle(const QPointF& aCenter,
                 qreal aRadius,
                 qreal aValue,
                 const QPen& aPen)
  {
    mCircleItems.push_back(addEllipse(aCenter.x() - aRadius + 0.5,
                                      aCenter.y() - aRadius + 0.5,
                                      2.0 * aRadius,
                                      2.0 * aRadius,
                                      aPen));

    AddCrossHair(aCenter, 10.0, 4.0, 0.0, QPen(aPen.color(), 0.0));

    QString vTextString("(" + QString::number(aCenter.x(),'f',1) + "/" + QString::number(aCenter.y(),'f',1) + ")\n"
                        "Score: " + QString::number(aValue,'f',1));

    QGraphicsTextItem* vTextItem = new QGraphicsTextItem(vTextString);
    QFont vFont = vTextItem->font();
    vFont.setPointSizeF(11.0);
    vTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    vTextItem->setFont(vFont);
    vTextItem->setPos(aCenter + QPointF(1,1));
    vTextItem->setDefaultTextColor(Qt::white);

    QGraphicsRectItem* vRectItem = new QGraphicsRectItem(vTextItem->boundingRect());
    vRectItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    vRectItem->setPos(aCenter + QPointF(1,1));
    vRectItem->setPen(Qt::NoPen);
    vRectItem->setBrush(QColor(0,0,0,127));

    addItem(vRectItem);
    mCircleItems.push_back(vRectItem);

    addItem(vTextItem);
    mCircleItems.push_back(vTextItem);
  }

  tROI* mROI;

  ggImageT<ggUInt16> mImageCamera;
  QGraphicsPixmapItem* mImageCameraPixmapItem;

  QGraphicsPixmapItem* mImageHoughPixmapItem;

  std::vector<QGraphicsItem*> mCircleItems;

};


#endif // GGGRAPHICSSCENE_H
