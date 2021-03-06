// 0) include own header
#include "ggUtilityQt.h"

// 1) include system
#include <QDebug>
#include <QBitmap>
#include <QPainter>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggNumerics.h"


int ggUtilityQt::GetColorMax(const QColor& aColor)
{
  return ggMax(aColor.red(), aColor.green(), aColor.blue());
}


qreal ggUtilityQt::GetColorMaxF(const QColor& aColor)
{
  return ggMax(aColor.redF(), aColor.greenF(), aColor.blueF());
}


qreal ggUtilityQt::GetColorLuma(const QColor& aColor)
{
  return 0.299 * aColor.redF() +
         0.587 * aColor.greenF() +
         0.114 * aColor.blueF();
}


QColor ggUtilityQt::GetColorSaturized(const QColor& aColor)
{
  qreal vColorBrightness = ggUtilityQt::GetColorMaxF(aColor);
  if (vColorBrightness != 0.0) {
    qreal vScale = 1.0 / vColorBrightness;
    return QColor::fromRgbF(ggClamp<qreal>(vScale * aColor.redF(), 0.0, 1.0),
                            ggClamp<qreal>(vScale * aColor.greenF(), 0.0, 1.0),
                            ggClamp<qreal>(vScale * aColor.blueF(), 0.0, 1.0),
                            aColor.alphaF());
  }
  else {
    QColor vColor(Qt::white);
    vColor.setAlphaF(aColor.alphaF());
    return vColor;
  }
}


QColor ggUtilityQt::GetColorScaled(const QColor& aColor, qreal aScale)
{
  return QColor::fromRgbF(ggClamp<qreal>(aScale * aColor.redF(), 0.0, 1.0),
                          ggClamp<qreal>(aScale * aColor.greenF(), 0.0, 1.0),
                          ggClamp<qreal>(aScale * aColor.blueF(), 0.0, 1.0),
                          aColor.alphaF());
}


QColor ggUtilityQt::GetColorInterpolated(const QColor& aColorA, const QColor& aColorB, qreal aWeightA)
{
  const qreal vWeightB = 1.0 - aWeightA;
  return QColor::fromRgbF(ggClamp<qreal>(aWeightA * aColorA.redF() + vWeightB * aColorB.redF(), 0.0, 1.0),
                          ggClamp<qreal>(aWeightA * aColorA.greenF() + vWeightB * aColorB.greenF(), 0.0, 1.0),
                          ggClamp<qreal>(aWeightA * aColorA.blueF() + vWeightB * aColorB.blueF(), 0.0, 1.0),
                          ggClamp<qreal>(aWeightA * aColorA.alphaF() + vWeightB * aColorB.alphaF(), 0.0, 1.0));
}


QColor ggUtilityQt::GetColorWithValue(const QColor& aColor, qreal aValue)
{
  qreal vH, vS, vV, vA;
  aColor.getHsvF(&vH, &vS, &vV, &vA);
  return QColor::fromHsvF(vH, vS, aValue, vA);
}


QColor ggUtilityQt::GetColorWithLightness(const QColor& aColor, qreal aLightness)
{
  qreal vH, vS, vL, vA;
  aColor.getHslF(&vH, &vS, &vL, &vA);
  return QColor::fromHslF(vH, vS, aLightness, vA);
}


QColor ggUtilityQt::GetColorContrast(const QColor& aColor)
{
  return (GetColorLuma(aColor) < 0.5) ? QColor(Qt::white) : QColor(Qt::black);
}


QColor ggUtilityQt::GetColorWithAlpha(const QColor& aColor, qreal aAlpha)
{
  return QColor::fromRgbF(aColor.redF(), aColor.greenF(), aColor.blueF(), aAlpha);
}


QColor ggUtilityQt::GetColorQt(const ggColorUInt8& aColor)
{
  return QColor(aColor.R(), aColor.G(), aColor.B(), aColor.A());
}


QRectF ggUtilityQt::GetRectInflated(const QRectF& aRect, qreal aDelta)
{
  return QRectF(aRect.x() - aDelta,
                aRect.y() - aDelta,
                aRect.width() + 2.0 * aDelta,
                aRect.height() + 2.0 * aDelta).normalized();
}


QString ggUtilityQt::ToString(const QColor& aColor)
{
  return "#" + QString::number(aColor.rgba(), 16);
}


bool ggUtilityQt::FromString(const QString& aColorARGB, QColor& aColor)
{
  bool vOK = false;
  if (aColorARGB.length() == 9) {
    QRgb vRgb = aColorARGB.right(8).toUInt(&vOK, 16);
    if (vOK) aColor.setRgba(vRgb);
  }
  return vOK;
}


template <typename T>
T ggUtilityQt::FromString(const QString& aString, bool* aOK)
{
  T vValue;
  bool vOK = FromString(aString, vValue);
  if (aOK != nullptr) *aOK = vOK;
  return vValue;
}


template QColor ggUtilityQt::FromString<QColor>(const QString& aString, bool* aOK);


QBrush ggUtilityQt::GetCheckerBoardBrush(int aSize, const QColor& aColor)
{
  QBitmap vBitmap(2*aSize, 2*aSize);
  QPainter vBitmapPainter(&vBitmap);
  vBitmapPainter.setPen(Qt::NoPen);
  vBitmapPainter.setBrush(Qt::black);
  vBitmapPainter.drawRect(    0,     0, aSize, aSize);
  vBitmapPainter.drawRect(aSize, aSize, aSize, aSize);
  vBitmapPainter.setBrush(Qt::white);
  vBitmapPainter.drawRect(aSize,     0, aSize, aSize);
  vBitmapPainter.drawRect(    0, aSize, aSize, aSize);
  QBrush vBrush(vBitmap);
  vBrush.setColor(aColor);
  return vBrush;
}


QPointF ggUtilityQt::GetPointF(const ggVector2Double& aVector)
{
  return QPointF(aVector.X(), aVector.Y());
}


QImage ggUtilityQt::GetImage(const ggImageT<ggUChar>& aImage,
                             const std::vector<ggColorUInt8>& aColorTable)
{
  QImage vImageQt(aImage.GetValues(),
                  static_cast<int>(aImage.GetSizeX()),
                  static_cast<int>(aImage.GetSizeY()),
                  static_cast<int>(aImage.GetSizeX()),
                  QImage::Format_Indexed8);

  QVector<QRgb> vColorTableQt;
  std::for_each(aColorTable.begin(), aColorTable.end(), [&vColorTableQt] (const ggColorUInt8& aColor) {
    vColorTableQt.push_back(ggUtilityQt::GetColorQt(aColor).rgba());
  });

  vImageQt.setColorTable(vColorTableQt);

  return vImageQt;
}
