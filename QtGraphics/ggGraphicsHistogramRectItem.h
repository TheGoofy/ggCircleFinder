#ifndef GGGRAPHICSHISTOGRAMRECTITEM_H
#define GGGRAPHICSHISTOGRAMRECTITEM_H

// 1) include system or QT
#include <QGraphicsRectItem>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggNumberTypes.h"

// 3) forward declarations
class ggHistogram;

class ggGraphicsHistogramRectItem :
  public QGraphicsRectItem
{
public:

  ggGraphicsHistogramRectItem(QGraphicsItem* aParent = nullptr);

  ggGraphicsHistogramRectItem(const QRectF& aRect,
                              const ggHistogram* aHistogram,
                              QGraphicsItem* aParent = nullptr);

  virtual ~ggGraphicsHistogramRectItem() override;

  void SetHistogram(const ggHistogram* aHistogram, bool aAdjustRect);
  void SetHistogramColor(const QColor& aColor);
  void SetLogarithmic(bool aLogarithmic);

protected:

  virtual void mousePressEvent(QGraphicsSceneMouseEvent* aEvent) override;

private:

  void Construct();

  void DeleteLineItems();
  void UpdateHistogram(bool aAdjustRect);
  qreal LogCount(ggInt64 aCount) const;

  bool mLogarithmic;
  QColor mHistogramColor;
  const ggHistogram* mHistogram;
  std::vector<QGraphicsLineItem*> mGridLineItems;
  std::vector<QGraphicsLineItem*> mBinLineItems;

};

#endif // GGGRAPHICSHISTOGRAMRECTITEM_H
