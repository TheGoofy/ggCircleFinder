// 0) include own header
#include "ggUtilityQt.h"

// 1) include system
#include <QDebug>
#include <QBitmap>
#include <QPainter>

// 2) include own project-related (sort by component dependency)


int ggUtilityQt::GetColorMax(const QColor& aColor)
{
  return ggUtility::Max(aColor.red(), aColor.green(), aColor.blue());
}


float ggUtilityQt::GetColorMaxF(const QColor& aColor)
{
  return ggUtility::Max(aColor.redF(), aColor.greenF(), aColor.blueF());
}


float ggUtilityQt::GetColorLuma(const QColor& aColor)
{
  return 0.299f * aColor.redF() +
         0.587f * aColor.greenF() +
         0.114f * aColor.blueF();
}


QColor ggUtilityQt::GetColorSaturized(const QColor& aColor)
{
  float vColorBrightness = ggUtilityQt::GetColorMaxF(aColor);
  if (vColorBrightness != 0.0f) {
    float vScale = 1.0f / vColorBrightness;
    return QColor::fromRgbF(ggUtility::Clamp<float>(vScale * aColor.redF(), 0.0f, 1.0f),
                            ggUtility::Clamp<float>(vScale * aColor.greenF(), 0.0f, 1.0f),
                            ggUtility::Clamp<float>(vScale * aColor.blueF(), 0.0f, 1.0f),
                            aColor.alphaF());
  }
  else {
    QColor vColor(Qt::white);
    vColor.setAlphaF(aColor.alphaF());
    return vColor;
  }
}


QColor ggUtilityQt::GetColorScaled(const QColor& aColor, float aScale)
{
  return QColor::fromRgbF(ggUtility::Clamp<float>(aScale * aColor.redF(), 0.0f, 1.0f),
                          ggUtility::Clamp<float>(aScale * aColor.greenF(), 0.0f, 1.0f),
                          ggUtility::Clamp<float>(aScale * aColor.blueF(), 0.0f, 1.0f),
                          aColor.alphaF());
}


QColor ggUtilityQt::GetColorInterpolated(const QColor& aColorA, const QColor& aColorB, float aWeightA)
{
  const float vWeightB = 1.0f - aWeightA;
  return QColor::fromRgbF(ggUtility::Clamp<float>(aWeightA * aColorA.redF() + vWeightB * aColorB.redF(), 0.0f, 1.0f),
                          ggUtility::Clamp<float>(aWeightA * aColorA.greenF() + vWeightB * aColorB.greenF(), 0.0f, 1.0f),
                          ggUtility::Clamp<float>(aWeightA * aColorA.blueF() + vWeightB * aColorB.blueF(), 0.0f, 1.0f),
                          ggUtility::Clamp<float>(aWeightA * aColorA.alphaF() + vWeightB * aColorB.alphaF(), 0.0f, 1.0f));
}


QColor ggUtilityQt::GetColorWithValue(const QColor& aColor, float aValue)
{
  qreal vH, vS, vV, vA;
  aColor.getHsvF(&vH, &vS, &vV, &vA);
  return QColor::fromHsvF(vH, vS, aValue, vA);
}


QColor ggUtilityQt::GetColorWithLightness(const QColor& aColor, float aLightness)
{
  qreal vH, vS, vL, vA;
  aColor.getHslF(&vH, &vS, &vL, &vA);
  return QColor::fromHslF(vH, vS, aLightness, vA);
}


QColor ggUtilityQt::GetColorContrast(const QColor& aColor)
{
  return (GetColorLuma(aColor) < 0.5f) ? QColor(Qt::white) : QColor(Qt::black);
}


QColor ggUtilityQt::GetColorWithAlpha(const QColor& aColor, float aAlpha)
{
  return QColor::fromRgbF(aColor.redF(), aColor.greenF(), aColor.blueF(), aAlpha);
}


QRectF ggUtilityQt::GetRectInflated(const QRectF& aRect, float aDelta)
{
  return QRectF(aRect.x() - aDelta,
                aRect.y() - aDelta,
                aRect.width() + 2.0f * aDelta,
                aRect.height() + 2.0f * aDelta).normalized();
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
