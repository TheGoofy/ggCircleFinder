#ifndef GGUTILITYQT_H
#define GGUTILITYQT_H

// 1) include system (or QT)
#include <QColor>
#include <QRect>
#include <QBrush>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggColorTypes.h"
#include "LibBase/ggUtility.h"
#include "LibImage/ggImageT.h"

// 3) forward declarations

/**
 * collection of general, simple utilities related with QT
 */
namespace ggUtilityQt
{

  int GetColorMax(const QColor& aColor);
  qreal GetColorMaxF(const QColor& aColor);
  qreal GetColorLuma(const QColor& aColor);

  QColor GetColorScaled(const QColor& aColor, qreal aScale);
  QColor GetColorInterpolated(const QColor& aColorA, const QColor& aColorB, qreal aWeightA);
  QColor GetColorContrast(const QColor& aColor);
  QColor GetColorSaturized(const QColor& aColor);
  QColor GetColorWithAlpha(const QColor& aColor, qreal aAlpha);
  QColor GetColorWithValue(const QColor& aColor, qreal aValue);
  QColor GetColorWithLightness(const QColor& aColor, qreal aLightness);
  QColor GetColorQt(const ggColorUInt8& aColor);

  QRectF GetRectInflated(const QRectF& aRect, qreal aDelta);

  QString ToString(const QColor& aColor);
  bool FromString(const QString& aColorARGB, QColor& aColor);

  template <typename T>
  T FromString(const QString& aString, bool* aOK = nullptr);

  QBrush GetCheckerBoardBrush(int aSize = 5, const QColor& aColor = QColor(170, 170, 170, 255));

  QPointF GetPointF(const ggVector2Double& aVector);

  QImage GetImage(const ggImageT<ggUChar>& aImage,
                  const std::vector<ggColorUInt8>& aColorTable);

}

#endif // GGUTILITYQT_H
