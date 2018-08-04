#ifndef GGGRAPHICSHISTOGRAMRECTITEM_H
#define GGGRAPHICSHISTOGRAMRECTITEM_H

// 1) include system or QT
#include <QGraphicsRectItem>

// 2) include own project-related (sort by component dependency)

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

  void SetHistogram(const ggHistogram* aHistogram);

private:

  void UpdateHistogram();

  const ggHistogram* mHistogram;
  std::vector<QGraphicsItem*> mItems;

};

#endif // GGGRAPHICSHISTOGRAMRECTITEM_H
