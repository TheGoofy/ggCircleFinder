#include "QtGraphics/ggGraphicsHistogramRectItem.h"
#include "LibImage/ggHistogram.h"

#include <math.h>
#include <QPen>
#include <QtDebug>
#include <QGraphicsSceneMouseEvent>


ggGraphicsHistogramRectItem::ggGraphicsHistogramRectItem(QGraphicsItem* aParent)
: QGraphicsRectItem(aParent),
  mHistogram(nullptr)
{
  Construct();
}


ggGraphicsHistogramRectItem::ggGraphicsHistogramRectItem(const QRectF& aRect,
                                                         const ggHistogram* aHistogram,
                                                         QGraphicsItem* aParent)
: QGraphicsRectItem(aRect, aParent),
  mHistogram(aHistogram)
{
  Construct();
}


ggGraphicsHistogramRectItem::~ggGraphicsHistogramRectItem()
{
  delete mHistogram;
}


void ggGraphicsHistogramRectItem::Construct()
{
  setFlag(ItemIgnoresTransformations);
  setPen(Qt::NoPen);
  setBrush(QColor(230, 230, 230, 120));
  mHistogramColor.setRgb(0, 0, 0, 255);
  mLogarithmic = true;
  UpdateHistogram(true);
}


void ggGraphicsHistogramRectItem::SetHistogram(const ggHistogram* aHistogram, bool aAdjustRect)
{
  if (aHistogram != mHistogram) {
    if (mHistogram != nullptr) delete mHistogram;
    mHistogram = aHistogram;
    UpdateHistogram(aAdjustRect);
  }
}


void ggGraphicsHistogramRectItem::SetHistogramColor(const QColor& aColor)
{
  if (aColor != mHistogramColor) {
    mHistogramColor = aColor;
    std::for_each(mBinLineItems.begin(), mBinLineItems.end(), [&aColor] (QGraphicsLineItem* aLineItem) {
      QPen vPen(aLineItem->pen());
      vPen.setColor(aColor);
      aLineItem->setPen(vPen);
    });
  }
}


void ggGraphicsHistogramRectItem::SetLogarithmic(bool aLogarithmic)
{
  if (aLogarithmic != mLogarithmic) {
    mLogarithmic = aLogarithmic;
    UpdateHistogram(false);
  }
}


void ggGraphicsHistogramRectItem::DeleteLineItems()
{
  std::for_each(mGridLineItems.begin(), mGridLineItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
  std::for_each(mBinLineItems.begin(), mBinLineItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
  mGridLineItems.clear();
  mBinLineItems.clear();
}


qreal ggGraphicsHistogramRectItem::LogCount(ggInt64 aCount) const
{
  return mLogarithmic ? log(1.0 + aCount) : aCount;
}


void ggGraphicsHistogramRectItem::UpdateHistogram(bool aAdjustRect)
{
  DeleteLineItems();
  if (mHistogram != nullptr) {

    // factors for aligning the histogram with rect()
    qreal vSize = mHistogram->GetNumberOfBins();
    if (aAdjustRect) setRect(QRectF(0, -vSize/2, vSize, vSize/2));
    ggInt64 vCountMax = mHistogram->GetCountMax();
    qreal vOffsetX = mHistogram->GetBinValueMinLowerF();
    qreal vScaleX = rect().width() / mHistogram->GetBinValueRangeOuterF();
    qreal vScaleY = rect().height() / LogCount(vCountMax);

    // draw some horizontal grid lines
    for (ggInt64 vIndexY = 0; vIndexY < 6; vIndexY++) {
      ggInt64 vCount = vIndexY * vCountMax / 6;
      ggDouble vPosY = rect().bottom() - vScaleY * LogCount(vCount);
      QGraphicsLineItem* vGridLineItem = new QGraphicsLineItem(rect().left(), vPosY, rect().right(), vPosY, this);
      vGridLineItem->setPen(QPen(QColor(0,0,0,50), 0.0, Qt::SolidLine, Qt::FlatCap));
      mGridLineItems.push_back(vGridLineItem);
    }

    // draw the bars for the bins
    for (ggInt64 vBinIndex = 0; vBinIndex < mHistogram->GetNumberOfBins(); vBinIndex++) {
      if (mHistogram->GetCount(vBinIndex) > 0) {
        ggDouble vPosX = rect().left() + vScaleX * (mHistogram->GetBinValueF(vBinIndex) - vOffsetX);
        ggDouble vPosY = rect().bottom() - vScaleY * LogCount(mHistogram->GetCount(vBinIndex));
        QGraphicsLineItem* vBinLineItem = new QGraphicsLineItem(vPosX, rect().bottom(), vPosX, vPosY, this);
        vBinLineItem->setPen(QPen(mHistogramColor, vScaleX * mHistogram->GetBinWidthF(vBinIndex), Qt::SolidLine, Qt::FlatCap));
        mBinLineItems.push_back(vBinLineItem);
      }
    }
  }
}


void ggGraphicsHistogramRectItem::mousePressEvent(QGraphicsSceneMouseEvent* /*aEvent*/)
{
  // togge logarithmic mode
  mLogarithmic = !mLogarithmic;
  UpdateHistogram(false);

  // no need for inherited event handling (objects in the background won't receive the event)
  // QGraphicsRectItem::mousePressEvent(aEvent);
}

