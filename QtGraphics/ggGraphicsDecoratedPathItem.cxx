// 0) include own header
#include "ggGraphicsDecoratedPathItem.h"

// 1) include system or QT
#include <QStyleOptionGraphicsItem>
#include <QPen>

// 2) include own project-related (sort by component dependency)
#include "QtGraphics/ggPainterPath.h"


ggGraphicsDecoratedPathItem::ggGraphicsDecoratedPathItem(QGraphicsItem* aParent) :
  QGraphicsPathItem(aParent),
  mConnectionValid(false),
  mDecorationItemSrc(nullptr),
  mDecorationItemDst(nullptr),
  mColorSelected(QColor(200, 255, 0, 255))
{
  setFlag(ItemIsSelectable);
  setCursor(Qt::CrossCursor);
  QPen vPen(pen());
  vPen.setCapStyle(Qt::FlatCap);
  vPen.setJoinStyle(Qt::MiterJoin);
  setPen(vPen);
  setBrush(Qt::NoBrush);
  setZValue(-1.0);
}


void ggGraphicsDecoratedPathItem::ClearConnection()
{
  setPath(QPainterPath());
  delete mDecorationItemSrc;
  delete mDecorationItemDst;
  mDecorationItemSrc = nullptr;
  mDecorationItemDst = nullptr;
  mConnectionValid = false;
}


void ggGraphicsDecoratedPathItem::SetConnection(const ggConnectionPoint& aPointSrc,
                                                const ggConnectionPoint& aPointDst)
{
  mPointSrc = aPointSrc;
  mPointDst = aPointDst;
  mConnectionValid = true;
  RebuildPath();
}


const ggDecoration& ggGraphicsDecoratedPathItem::DecorationSrc() const
{
  return mDecorationSrc;
}


const ggDecoration& ggGraphicsDecoratedPathItem::DecorationDst() const
{
  return mDecorationDst;
}


void ggGraphicsDecoratedPathItem::SetDecorationSrc(ggDecoration::cType aType,
                                                   qreal aLength,
                                                   ggDecoration::cFill aFill)
{
  mDecorationSrc.Set(aType, aLength, aFill);
  RebuildPath();
}


void ggGraphicsDecoratedPathItem::SetDecorationDst(ggDecoration::cType aType,
                                                   qreal aLength,
                                                   ggDecoration::cFill aFill)
{
  mDecorationDst.Set(aType, aLength, aFill);
  RebuildPath();
}


void ggGraphicsDecoratedPathItem::setPen(const QPen& aPen)
{
  mColorNormal = aPen.color();
  QGraphicsPathItem::setPen(aPen);
  RebuildPath();
}


void ggGraphicsDecoratedPathItem::SetColorSelected(const QColor& aColor)
{
  if (mColorSelected != aColor) {
    mColorSelected = aColor;
    QGraphicsPathItem::setPen(GetPen());
    RebuildPath();
  }
}


const QColor& ggGraphicsDecoratedPathItem::GetColorSelected() const
{
  return mColorSelected;
}


QPen ggGraphicsDecoratedPathItem::GetPen() const
{
  QPen vPen(pen());
  vPen.setColor(isSelected() ? mColorSelected : mColorNormal);
  return vPen;
}


QVariant ggGraphicsDecoratedPathItem::itemChange(GraphicsItemChange aChange,
                                                 const QVariant& aValue)
{
  // change color when selected
  if (aChange == ItemSelectedHasChanged) {
    QGraphicsPathItem::setPen(GetPen());
    RebuildPath();
  }

  // execute base item change
  return QGraphicsPathItem::itemChange(aChange, aValue);
}


void ggGraphicsDecoratedPathItem::paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget)
{
  // don't draw the (ugly) default selection box
  QStyle::State vState = aOption->state;
  const_cast<QStyleOptionGraphicsItem*>(aOption)->state &= !QStyle::State_Selected;
  QGraphicsPathItem::paint(aPainter, aOption, aWidget);
  const_cast<QStyleOptionGraphicsItem*>(aOption)->state = vState;
}


QPainterPath ggGraphicsDecoratedPathItem::shape() const
{
  QPainterPathStroker vPathStroker(pen());
  return vPathStroker.createStroke(path()).simplified();
}


QPainterPath ggGraphicsDecoratedPathItem::GetPath(const ggConnectionPoint& aPointSrc,
                                                  const ggConnectionPoint& aPointDst) const
{
  // calculate the key points
  QPointF vPointSrcBase = aPointSrc.GetControlPoint(mDecorationSrc.GetLength());
  QPointF vPointDstBase = aPointDst.GetControlPoint(mDecorationDst.GetLength());
  qreal vDistance = QLineF(vPointSrcBase, vPointDstBase).length();
  QPointF vPointSrcControl = aPointSrc.GetControlPoint(mDecorationSrc.GetLength() + vDistance/2.5);
  QPointF vPointDstControl = aPointDst.GetControlPoint(mDecorationDst.GetLength() + vDistance/2.5);

  // some decorations need a bit a longer path
  bool vExtraSrcPath = ggPainterPath::ConnectToCenter(mDecorationSrc.GetType(), mDecorationSrc.GetFill());
  bool vExtraDstPath = ggPainterPath::ConnectToCenter(mDecorationDst.GetType(), mDecorationDst.GetFill());
  QPointF vPointSrcExtraBase = vExtraSrcPath ? aPointSrc.GetControlPoint(mDecorationSrc.GetLength()/2.0) : vPointSrcBase;
  QPointF vPointDstExtraBase = vExtraDstPath ? aPointDst.GetControlPoint(mDecorationDst.GetLength()/2.0) : vPointDstBase;

  // compile path
  QPainterPath vPath;
  vPath.moveTo(vPointSrcExtraBase);
  vPath.lineTo(vPointSrcBase);
  vPath.cubicTo(vPointSrcControl,
                vPointDstControl,
                vPointDstBase);
  vPath.lineTo(vPointDstExtraBase);

  return vPath;
}


QGraphicsItem* ggGraphicsDecoratedPathItem::CreateDecoration(const ggConnectionPoint& aPoint,
                                                             const ggDecoration& aDecoration)
{
  QPen vPen(pen());
  vPen.setStyle(Qt::SolidLine);
  if (aDecoration.GetFillSolid() &&
      ggPainterPath::IsClosed(aDecoration.GetType())) {
    vPen = Qt::NoPen;
  }

  QBrush vBrush(pen().color());
  if (aDecoration.GetFillEmpty() ||
      ggPainterPath::IsOpen(aDecoration.GetType())) {
    vBrush = Qt::NoBrush;
  }

  ggPainterPath vPath;
  vPath.AddDecoration(aDecoration, aPoint.GetPosition(), aPoint.GetDirection());

  QGraphicsPathItem* vPathItem = new QGraphicsPathItem(this);
  vPathItem->setPen(vPen);
  vPathItem->setBrush(vBrush);
  vPathItem->setPath(vPath);

  return vPathItem;
}


void ggGraphicsDecoratedPathItem::RebuildPath()
{
  // don't do anyting, if the connection is not valid
  if (!mConnectionValid) return;

  // update path
  setPath(GetPath(mPointSrc, mPointDst));

  // update decoration
  delete mDecorationItemSrc;
  delete mDecorationItemDst;
  mDecorationItemSrc = CreateDecoration(mPointSrc, mDecorationSrc);
  mDecorationItemDst = CreateDecoration(mPointDst, mDecorationDst);
}
