#ifndef GGGRAPHICSDECORATEDPATHITEM_H
#define GGGRAPHICSDECORATEDPATHITEM_H

// 1) include system or QT
#include <QGraphicsPathItem>

// 2) include own project-related (sort by component dependency)
#include "QtGraphics/ggConnectionPoint.h"
#include "QtGraphics/ggDecoration.h"

// 3) forward declarations

/**
 * @brief The ggGraphicsDecoratedPathItem class
 */
class ggGraphicsDecoratedPathItem :
  public QGraphicsPathItem
{
public:

  ggGraphicsDecoratedPathItem(QGraphicsItem* aParent = nullptr);

  void ClearConnection();
  void SetConnection(const ggConnectionPoint& aPointSrc,
                     const ggConnectionPoint& aPointDst);

  const ggDecoration& DecorationSrc() const;
  const ggDecoration& DecorationDst() const;

  void SetDecorationSrc(ggDecoration::cType aType,
                        qreal aLength = 12.0,
                        ggDecoration::cFill aFill = ggDecoration::cFill::eSolid);

  void SetDecorationDst(ggDecoration::cType aType,
                        qreal aLength = 12.0,
                        ggDecoration::cFill aFill = ggDecoration::cFill::eSolid);

  void setPen(const QPen& aPen);
  void SetColorSelected(const QColor& aColor);
  const QColor& GetColorSelected() const;

protected:

  virtual QVariant itemChange(GraphicsItemChange aChange,
                              const QVariant& aValue) override;

  virtual void paint(QPainter* aPainter,
                     const QStyleOptionGraphicsItem* aOption,
                     QWidget* aWidget) override;

  virtual QPainterPath shape() const override;

private:

  QPen GetPen() const;

  QPainterPath GetPath(const ggConnectionPoint& aPointSrc,
                       const ggConnectionPoint& aPointDst) const;

  QGraphicsItem* CreateDecoration(const ggConnectionPoint& aPoint,
                                  const ggDecoration& aDecoration);

  void RebuildPath();

  // the connection points
  bool mConnectionValid;
  ggConnectionPoint mPointSrc;
  ggConnectionPoint mPointDst;

  // connection decoration properties
  ggDecoration mDecorationSrc;
  ggDecoration mDecorationDst;

  // decoraton graph items
  QGraphicsItem* mDecorationItemSrc;
  QGraphicsItem* mDecorationItemDst;

  // selection
  QColor mColorNormal;
  QColor mColorSelected;

};

#endif // GGGRAPHICSDECORATEDPATHITEM_H
