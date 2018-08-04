#include "QtGraphics/ggGraphicsHistogramRectItem.h"
#include "LibImage/ggHistogram.h"

#include <math.h>
#include <QPen>
#include <QtDebug>


ggGraphicsHistogramRectItem::ggGraphicsHistogramRectItem(QGraphicsItem* aParent)
: QGraphicsRectItem(aParent),
  mHistogram(nullptr)
{
  /*
  QRectF vRect(-10, -10, 20, 20);
  QGraphicsLineItem* vLineItem1 = new QGraphicsLineItem(QLineF(vRect.topLeft(), vRect.bottomRight()), this);
  QGraphicsLineItem* vLineItem2 = new QGraphicsLineItem(QLineF(vRect.topRight(), vRect.bottomLeft()), this);
  */
}


ggGraphicsHistogramRectItem::ggGraphicsHistogramRectItem(const QRectF& aRect,
                                                         const ggHistogram* aHistogram,
                                                         QGraphicsItem* aParent)
: QGraphicsRectItem(aRect, aParent),
  mHistogram(aHistogram)
{
  UpdateHistogram();
}


ggGraphicsHistogramRectItem::~ggGraphicsHistogramRectItem()
{
  delete mHistogram;
}


void ggGraphicsHistogramRectItem::SetHistogram(const ggHistogram* aHistogram)
{
  if (aHistogram != mHistogram) {
    if (mHistogram != nullptr) delete mHistogram;
    mHistogram = aHistogram;
    UpdateHistogram();
  }
}


void ggGraphicsHistogramRectItem::UpdateHistogram()
{
  std::for_each(mItems.begin(), mItems.end(), [] (QGraphicsItem* aItem) { delete aItem; });
  mItems.clear();
  if (mHistogram != nullptr) {
    // qDebug() << __PRETTY_FUNCTION__ << pos() << rect() << opacity();
    QRectF vRect(rect());
    qreal vOffsetX = mHistogram->GetBinValueMinLowerF();
    qreal vScaleX = vRect.width() / mHistogram->GetBinValueRangeOuterF();
    qreal vScaleY = vRect.height() / log(1.0 + mHistogram->GetCountMax());
    QColor vColor(pen().color()); vColor.setAlpha(255);
    for (ggInt64 vBinIndex = 0; vBinIndex < mHistogram->GetNumberOfBins(); vBinIndex++) {
      if (mHistogram->GetCount(vBinIndex) > 0) {
        ggDouble vPosX = vRect.left() + vScaleX * (mHistogram->GetBinValueF(vBinIndex) - vOffsetX);
        ggDouble vPosY = vRect.bottom() - vScaleY * log(1.0 + mHistogram->GetCount(vBinIndex));
        QGraphicsLineItem* vLineItem = new QGraphicsLineItem(vPosX, vRect.bottom(), vPosX, vPosY, this);
        QPen vPen(vColor, vScaleX * mHistogram->GetBinWidthF(vBinIndex));
        vPen.setCapStyle(Qt::FlatCap);
        vLineItem->setPen(vPen);
        mItems.push_back(vLineItem);
      }
    }
  }
}
