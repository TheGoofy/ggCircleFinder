// 0) include own header
#include "ggPainterPath.h"

// 1) include system or QT
#include <QVector2D>

// 2) include own project-related (sort by component dependency)


ggPainterPath::ggPainterPath() :
  QPainterPath(),
  mDecorationRatio(0.0)
{
}


void ggPainterPath::SetDecorationRatio(qreal aRatio)
{
  mDecorationRatio = aRatio;
}


qreal ggPainterPath::GetDecorationRatio() const
{
  return mDecorationRatio;
}


qreal ggPainterPath::GetDecorationRatio(ggDecoration::cType aType) const
{
  if (mDecorationRatio != 0.0) return mDecorationRatio;
  switch (aType) {
    case ggDecoration::cType::eNothing: return 0.0;
    case ggDecoration::cType::eLine: return 0.0;
    case ggDecoration::cType::eArrow: return 0.8;
    case ggDecoration::cType::eArrowBack: return 0.8;
    case ggDecoration::cType::eTriangle: return 0.8;
    case ggDecoration::cType::eTriangleBack: return 0.8;
    case ggDecoration::cType::eDiamond: return 0.7;
    case ggDecoration::cType::eCross: return 1.0;
    case ggDecoration::cType::eCircle: return 1.0;
  }
}


void ggPainterPath::AddDecoration(const ggDecoration& aDecoration,
                                  const QPointF& aEnd,
                                  const QVector2D& aDirection)
{
  QPointF vStart(aEnd + (static_cast<float>(aDecoration.GetLength()) * aDirection.normalized()).toPointF());
  AddDecoration(aDecoration.GetType(), vStart, aEnd);
}


void ggPainterPath::AddDecoration(ggDecoration::cType aType,
                                  const QPointF& aStart,
                                  const QPointF& aEnd)
{
  switch (aType) {
    case ggDecoration::cType::eNothing: break;
    case ggDecoration::cType::eLine: AddLine(aStart, aEnd); break;
    case ggDecoration::cType::eArrow: AddArrow(aStart, aEnd); break;
    case ggDecoration::cType::eArrowBack: AddArrowBack(aStart, aEnd); break;
    case ggDecoration::cType::eTriangle: AddTriangle(aStart, aEnd); break;
    case ggDecoration::cType::eTriangleBack: AddTriangleBack(aStart, aEnd); break;
    case ggDecoration::cType::eDiamond: AddDiamond(aStart, aEnd); break;
    case ggDecoration::cType::eCross: AddCross(aStart, aEnd); break;
    case ggDecoration::cType::eCircle: AddCircle(aStart, aEnd); break;
  }
}


bool ggPainterPath::IsClosed(ggDecoration::cType aType)
{
  return (aType == ggDecoration::cType::eTriangle ||
          aType == ggDecoration::cType::eTriangleBack ||
          aType == ggDecoration::cType::eDiamond ||
          aType == ggDecoration::cType::eCircle);
}


bool ggPainterPath::IsOpen(ggDecoration::cType aType)
{
  return !IsClosed(aType);
}


bool ggPainterPath::ConnectToCenter(ggDecoration::cType aType,
                                    ggDecoration::cFill aFill)
{
  if (aFill == ggDecoration::cFill::eEmpty) {
    return (aType == ggDecoration::cType::eArrow ||
            aType == ggDecoration::cType::eCross);
  }
  if (aFill == ggDecoration::cFill::eSolid) {
    return (aType == ggDecoration::cType::eArrow ||
            aType == ggDecoration::cType::eTriangleBack ||
            aType == ggDecoration::cType::eDiamond ||
            aType == ggDecoration::cType::eCross ||
            aType == ggDecoration::cType::eCircle);
  }
  return false;
}


QPointF ggPainterPath::CalculateCenter(const QPointF& aStart,
                                       const QPointF& aEnd) const
{
  return (aStart + aEnd) / 2.0;
}


void ggPainterPath::CalculateDimensions(ggDecoration::cType aType,
                                        const QPointF& aStart,
                                        const QPointF& aEnd,
                                        QVector2D& aDirection,
                                        QVector2D& aNormal,
                                        float& aLength,
                                        float& aWidth2) const
{
  aDirection = QVector2D(aEnd - aStart);
  aLength = aDirection.length();
  if (aLength != 0.0f) aDirection /= aLength;
  aNormal = QVector2D(aDirection.y(), -aDirection.x());
  aWidth2 = aLength * static_cast<float>(GetDecorationRatio(aType)) / 2.0f;
}


void ggPainterPath::AddLine(const QPointF& aStart, const QPointF& aEnd)
{
  moveTo(aStart);
  lineTo(aEnd);
}


void ggPainterPath::AddArrow(const QPointF& aStart, const QPointF& aEnd)
{
  QPointF vCenter = CalculateCenter(aStart, aEnd);
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eArrow, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aStart + (vWidth2 * vNorm).toPointF());
  lineTo(aEnd);
  lineTo(aStart - (vWidth2 * vNorm).toPointF());
  moveTo(vCenter);
  lineTo(aEnd);
}


void ggPainterPath::AddArrowBack(const QPointF& aStart, const QPointF& aEnd)
{
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eArrowBack, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aEnd + (vWidth2 * vNorm).toPointF());
  lineTo(aStart);
  lineTo(aEnd - (vWidth2 * vNorm).toPointF());
  moveTo(aEnd);
}


void ggPainterPath::AddTriangle(const QPointF& aStart, const QPointF& aEnd)
{
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eTriangle, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aEnd);
  lineTo(aStart + (vWidth2 * vNorm).toPointF());
  lineTo(aStart - (vWidth2 * vNorm).toPointF());
  lineTo(aEnd);
}


void ggPainterPath::AddTriangleBack(const QPointF& aStart, const QPointF& aEnd)
{
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eTriangleBack, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aStart);
  lineTo(aEnd + (vWidth2 * vNorm).toPointF());
  lineTo(aEnd - (vWidth2 * vNorm).toPointF());
  lineTo(aStart);
  moveTo(aEnd);
}


void ggPainterPath::AddDiamond(const QPointF& aStart, const QPointF& aEnd)
{
  QPointF vCenter = CalculateCenter(aStart, aEnd);
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eDiamond, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aEnd);
  lineTo(vCenter + (vWidth2 * vNorm).toPointF());
  lineTo(aStart);
  lineTo(vCenter - (vWidth2 * vNorm).toPointF());
  lineTo(aEnd);
}


void ggPainterPath::AddCross(const QPointF& aStart, const QPointF& aEnd)
{
  QVector2D vDirection, vNorm; float vLength, vWidth2;
  CalculateDimensions(ggDecoration::cType::eCross, aStart, aEnd, vDirection,
                      vNorm, vLength, vWidth2);

  moveTo(aStart + (vWidth2 * vNorm).toPointF());
  lineTo(aEnd - (vWidth2 * vNorm).toPointF());
  moveTo(aStart - (vWidth2 * vNorm).toPointF());
  lineTo(aEnd + (vWidth2 * vNorm).toPointF());
  moveTo(aEnd);
}


void ggPainterPath::AddCircle(const QPointF& aStart, const QPointF& aEnd)
{
  qreal vRadius = QLineF(aStart, aEnd).length() / 2.0;
  QPointF vCenter = CalculateCenter(aStart, aEnd);
  addEllipse(vCenter, vRadius, vRadius);
}
