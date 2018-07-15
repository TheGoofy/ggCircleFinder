#ifndef GGGRAPHICSMANIPULATORITEM_H
#define GGGRAPHICSMANIPULATORITEM_H

// 1) include system or QT
#include <QGraphicsRectItem>
#include <QCursor>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggObserver.h"
#include "QtGraphics/ggGraphicsHandleItemT.h"

// 3) forward declarations

/**
 *
 */
template <typename TBaseItem = QGraphicsRectItem,
          typename THandleItem = ggGraphicsHandleItemEllipse>
class ggGraphicsManipulatorRectItemT :
  public TBaseItem,
  public ggObserver
{

public:

  ggGraphicsManipulatorRectItemT(const QRectF& aRect,
                                 const float& aHandleSize = 9.0f,
                                 QGraphicsItem* aParent = nullptr) :
    TBaseItem(aRect, aParent)
  {
    TBaseItem::setFlag(TBaseItem::ItemIsMovable);
    TBaseItem::setCursor(Qt::SizeAllCursor);
    TBaseItem::setAcceptHoverEvents(true);
    TBaseItem::setFlag(TBaseItem::ItemSendsGeometryChanges);

    mHandleItemTL = new THandleItem(aHandleSize, this);
    mHandleItemTR = new THandleItem(aHandleSize, this);
    mHandleItemBR = new THandleItem(aHandleSize, this);
    mHandleItemBL = new THandleItem(aHandleSize, this);

    mHandleItemTL->setCursor(Qt::SizeFDiagCursor);
    mHandleItemTR->setCursor(Qt::SizeBDiagCursor);
    mHandleItemBR->setCursor(Qt::SizeFDiagCursor);
    mHandleItemBL->setCursor(Qt::SizeBDiagCursor);

    mHandleItemTL->LinkY(mHandleItemTR);
    mHandleItemTR->LinkX(mHandleItemBR);
    mHandleItemBR->LinkY(mHandleItemBL);
    mHandleItemBL->LinkX(mHandleItemTL);

    mHandleItemTL->setPos(TBaseItem::rect().topLeft());
    mHandleItemBR->setPos(TBaseItem::rect().bottomRight());

    Attach(mHandleItemTL->GetSubjectPosition());
    Attach(mHandleItemBR->GetSubjectPosition());

    HideHandles();
  }

  virtual ~ggGraphicsManipulatorRectItemT() {
  }

  const ggSubject* GetSubjectPosition() const {
    return &mSubjectPosition;
  }

  QPointF GetPosition() const {
    return TBaseItem::pos() + TBaseItem::rect().topLeft();
  }

  void SetPosition(const QPointF& aPosition) {
    ggSubject::cExecutorBlocking vBlocker(&mSubjectPosition);
    TBaseItem::setPos(aPosition);
    QRectF vRect(TBaseItem::rect());
    vRect.moveTo(0.0f, 0.0f);
    TBaseItem::setRect(vRect);
    UpdateHandlePositions();
  }

  const ggSubject* GetSubjectSize() const {
    return &mSubjectSize;
  }

  QSizeF GetSize() const {
    return TBaseItem::rect().size();
  }

  void SetSize(const QSizeF& aSize) {
    QRectF vRect(TBaseItem::rect());
    vRect.setSize(aSize);
    TBaseItem::setRect(vRect);
    UpdateHandlePositions();
  }

  void SetHandleColor(const QColor& aColor) {
    if (mHandleColor != aColor) {
      mHandleColor = aColor;
    }
  }

protected:

  void hoverEnterEvent(QGraphicsSceneHoverEvent* aEvent) override {
    ShowHandles();
    TBaseItem::hoverEnterEvent(aEvent);
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* aEvent) override {
    HideHandles();
    TBaseItem::hoverLeaveEvent(aEvent);
  }

  QVariant itemChange(typename TBaseItem::GraphicsItemChange aChange,
                      const QVariant& aValue) override {
    // notify position change
    if (aChange == TBaseItem::ItemPositionHasChanged) {
      mSubjectPosition.Notify();
    }
    // execute base item change
    return TBaseItem::itemChange(aChange, aValue);
  }

  virtual void Update(const ggSubject* aSubject) override {
    if (aSubject == mHandleItemTL->GetSubjectPosition() ||
        aSubject == mHandleItemBR->GetSubjectPosition()) {
      UpdateRect();
      mSubjectSize.Notify();
    }
  }

private:

  void ShowHandles() {
    SetHandleBrush(mHandleColor);
  }

  void HideHandles() {
    SetHandleBrush(Qt::transparent);
  }

  void SetHandleBrush(const QBrush& aBrush) {
    mHandleItemTL->setBrush(aBrush);
    mHandleItemTR->setBrush(aBrush);
    mHandleItemBR->setBrush(aBrush);
    mHandleItemBL->setBrush(aBrush);
  }

  void UpdateHandlePositions() {
    ggObserver::cExecutorBlocking vBlockerTL(this, mHandleItemTL->GetSubjectPosition());
    ggObserver::cExecutorBlocking vBlockerBR(this, mHandleItemTL->GetSubjectPosition());
    mHandleItemTL->setPos(TBaseItem::rect().topLeft());
    mHandleItemBR->setPos(TBaseItem::rect().bottomRight());
  }

  void UpdateRect() {
    TBaseItem::setRect(QRectF(mHandleItemTL->pos(), mHandleItemBR->pos()).normalized());
  }

  QColor mHandleColor;
  THandleItem* mHandleItemTL;
  THandleItem* mHandleItemTR;
  THandleItem* mHandleItemBR;
  THandleItem* mHandleItemBL;

  ggSubject mSubjectPosition;
  ggSubject mSubjectSize;

};

#endif // GGGRAPHICSMANIPULATORITEM_H
