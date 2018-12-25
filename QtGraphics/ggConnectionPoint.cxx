#define _USE_MATH_DEFINES

// 0) include own header
#include "ggConnectionPoint.h"

// 1) include system or QT
#include <math.h>

// 2) include own project-related (sort by component dependency)


ggConnectionPoint::ggConnectionPoint() :
  mPosition(0.0, 0.0),
  mDirection(1.0, 0.0)
{
}


void ggConnectionPoint::SetPosition(const QPointF& aPosition)
{
  mPosition = aPosition;
}


const QPointF& ggConnectionPoint::GetPosition() const
{
  return mPosition;
}


void ggConnectionPoint::SetDirection(const QVector2D& aDirection)
{
  if (!aDirection.isNull()) mDirection = aDirection.normalized();
}


const QVector2D& ggConnectionPoint::GetDirection() const
{
  return mDirection;
}


qreal ggConnectionPoint::GetDirectionAngle() const
{
  if (mDirection.isNull()) return 0.0;
  return atan2(mDirection.y(), mDirection.x());
}


void ggConnectionPoint::SetDirectionAngle(qreal aAngle)
{
  mDirection.setX(static_cast<float>(cos(aAngle)));
  mDirection.setY(static_cast<float>(sin(aAngle)));
}


void ggConnectionPoint::SetDirectionRight()
{
  SetDirectionAngle(0.0);
}


void ggConnectionPoint::SetDirectionLeft()
{
  SetDirectionAngle(M_PI);
}


void ggConnectionPoint::SetDirectionUp()
{
  SetDirectionAngle(-M_PI_2);
}


void ggConnectionPoint::SetDirectionDown()
{
  SetDirectionAngle(M_PI_2);
}


QPointF ggConnectionPoint::GetControlPoint(qreal aDistance) const
{
  QPointF vControlPoint(mPosition);
  vControlPoint += (static_cast<float>(aDistance) * mDirection).toPointF();
  return vControlPoint;
}
