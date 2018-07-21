#ifndef GGGRAPHICSCONNECTIONPOINTITEM_H
#define GGGRAPHICSCONNECTIONPOINTITEM_H

// 1) include system or QT
#include <QGraphicsEllipseItem>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggObserver.h"
#include "QtGraphics/ggSubjectConnectionPoint.h"

// 3) forward declarations

/**
 * @brief The ggGraphicsConnectionPointItem class
 */
class ggGraphicsConnectionPointItem :
  public QGraphicsEllipseItem,
  private ggObserver
{

public:

  ggGraphicsConnectionPointItem(qreal aSize = 11.0,
                                QGraphicsItem* aParent = nullptr);

  void SetSize(qreal aSize);
  qreal GetSize() const;

  void SetPointPosition(const QPointF& aPosition);

  void SetPointDirection(qreal aAngle);
  void SetPointDirection(const QVector2D& aDirection);
  void SetPointDirectionRight();
  void SetPointDirectionLeft();
  void SetPointDirectionUp();
  void SetPointDirectionDown();

  const ggSubjectConnectionPoint* GetSubjectConnectionPoint() const;

protected:

  virtual void Update(const ggSubject* aSubject) override;

  virtual QVariant itemChange(GraphicsItemChange aChange,
                              const QVariant& aValue) override;

  virtual void wheelEvent(QGraphicsSceneWheelEvent* aEvent) override;

private:

  void UpdateConnectionPoint();

  ggSubjectConnectionPoint mConnectionPoint;
  QGraphicsLineItem* mDirectionLine;

};

#endif // GGGRAPHICSCONNECTIONPOINTITEM_H
