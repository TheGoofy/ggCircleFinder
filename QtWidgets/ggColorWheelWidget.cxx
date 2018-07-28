// 0) include own header
#include "ggColorWheelWidget.h"

// 1) include system or QT
#include <math.h>
#include <QPainter>
#include <QMouseEvent>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggNumerics.h"
#include "QtBase/ggUtilityQt.h"


ggColorWheelWidget::ggColorWheelWidget(QWidget* aParent) :
  QWidget(aParent),
  mColorValue(0.0),
  mAdaptToValue(false),
  mSelectorRadius(3.0),
  mMouseDragging(false)
{
  setMouseTracking(true);
  SetColor(QColor(200, 150, 50, 255));
  UpdateCornerColors();
}


void ggColorWheelWidget::SetColor(const QColor& aColor)
{
  // calculate saturized color and brightness
  qreal vColorValue = aColor.valueF();
  QColor vColorSaturized = ggUtilityQt::GetColorSaturized(aColor);

  // update colors, if changed
  if (vColorValue != mColorValue ||
      vColorSaturized != mColorSaturized) {
    mColorSaturized = vColorSaturized;
    mColorValue = vColorValue;
    mColorPosition = GetPosition(mColorSaturized);
    UpdateCornerColors();
    update(); // trigger repaint
  }
}


QColor ggColorWheelWidget::GetColor() const
{
  return ggUtilityQt::GetColorScaled(mColorSaturized, mColorValue);
}


void ggColorWheelWidget::SetAdaptToValue(bool aAdapt)
{
  if (mAdaptToValue != aAdapt) {
    mAdaptToValue = aAdapt;
    UpdateCornerColors();
    update(); // trigger repaint
  }
}


bool ggColorWheelWidget::GetAdaptToValue() const
{
  return mAdaptToValue;
}


void ggColorWheelWidget::UpdateCornerColors()
{
  qreal vVal = mAdaptToValue ? mColorValue : 1.0;
  mColorR = QColor::fromRgbF(vVal,  0.0,  0.0); // red
  mColorY = QColor::fromRgbF(vVal, vVal,  0.0); // yellow
  mColorG = QColor::fromRgbF( 0.0, vVal,  0.0); // green
  mColorC = QColor::fromRgbF( 0.0, vVal, vVal); // cyan
  mColorB = QColor::fromRgbF( 0.0,  0.0, vVal); // blue
  mColorM = QColor::fromRgbF(vVal,  0.0, vVal); // magenta
}


static QPointF GetPosition(qreal aScale, qreal aColorA, qreal aColorB,
                           const QPointF& aPosition,
                           const QVector2D& aDirectionA,
                           const QVector2D& aDirectionB)
{
  if (aScale != 0.0) {
    float vFactorA = static_cast<float>(1.0 - aColorA / aScale);
    float vFactorB = static_cast<float>(1.0 - aColorB / aScale);
    return aPosition - (vFactorA * aDirectionA).toPointF()
                     - (vFactorB * aDirectionB).toPointF();
  }
  else {
    return aPosition;
  }
}


QPointF ggColorWheelWidget::GetPosition(const QColor& aColor) const
{
  if (ggUtilityQt::GetColorMax(aColor) == aColor.red()) {
    return ::GetPosition(aColor.redF(), aColor.greenF(), aColor.blueF(), mPosCenter, mDirG, mDirB);
  }
  if (ggUtilityQt::GetColorMax(aColor) == aColor.green()) {
    return ::GetPosition(aColor.greenF(), aColor.blueF(), aColor.redF(), mPosCenter, mDirB, mDirR);
  }
  if (ggUtilityQt::GetColorMax(aColor) == aColor.blue()) {
    return ::GetPosition(aColor.blueF(), aColor.redF(), aColor.greenF(), mPosCenter, mDirR, mDirG);
  }
  return QPointF(mPosCenter);
}


static void GetFactors(const QVector2D& aDirectionA,
                       const QVector2D& aDirectionB,
                       const QVector2D& aPosition,
                       qreal& aFactorA,
                       qreal& aFactorB)
{
  float vArea = aDirectionA.x() * aDirectionB.y() - aDirectionA.y() * aDirectionB.x();
  if (vArea != 0.0f) {
    aFactorA = static_cast<qreal>((aDirectionB.y() * aPosition.x() - aDirectionB.x() * aPosition.y()) / vArea);
    aFactorB = static_cast<qreal>((aDirectionA.x() * aPosition.y() - aDirectionA.y() * aPosition.x()) / vArea);
  }
}


float SignFromLine(const QPointF& aLineOrigin,
                   const QVector2D& aLineDirP1,
                   const QVector2D& aLineDirP2,
                   const QPointF& aPosition)
{
  QVector2D vLineDirection(aLineDirP2 - aLineDirP1);
  QVector2D vLineDirectionN(-vLineDirection.y(), vLineDirection.x());
  QVector2D vDirectionP(aPosition - aLineOrigin - aLineDirP1.toPointF());
  return QVector2D::dotProduct(vDirectionP, vLineDirectionN);
}


QPointF ClampPosition(const QPointF& aPosition,
                      const QPointF& aCenter,
                      const QVector2D& aDirA,
                      const QVector2D& aDirB)
{
  if (SignFromLine(aCenter, aDirA, aDirB, aPosition) > 0.0f) {
    if (SignFromLine(aCenter, QVector2D(), aDirA, aPosition) <= 0.0f) {
      if (SignFromLine(aCenter, QVector2D(), aDirB, aPosition) >= 0.0f) {
        QVector2D vDirAB(aDirB - aDirA);
        float vLengthAB = vDirAB.length();
        if (vLengthAB != 0.0f) vDirAB /= vLengthAB;
        QVector2D vDirAP(aPosition - aCenter - aDirA.toPointF());
        float vScaleAB = QVector2D::dotProduct(vDirAB, vDirAP);
        vScaleAB = ggClamp(vScaleAB, 0.0f, vLengthAB);
        return aCenter + (aDirA + vScaleAB * vDirAB).toPointF();
      }
    }
  }
  return aPosition;
}


QPointF ggColorWheelWidget::ClampPosition(const QPointF& aPosition) const
{
  QPointF vPosition(aPosition);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirR, mDirY);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirY, mDirG);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirG, mDirC);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirC, mDirB);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirB, mDirM);
  vPosition = ::ClampPosition(vPosition, mPosCenter, mDirM, mDirR);
  return vPosition;
}


QColor ggColorWheelWidget::GetColorSaturized(const QPointF& aPosition) const
{
  QVector2D vDir(ClampPosition(aPosition) - mPosCenter);
  qreal vScaleR1 = 0.0; qreal vScaleR2 = 0.0;
  qreal vScaleG1 = 0.0; qreal vScaleG2 = 0.0;
  qreal vScaleB1 = 0.0; qreal vScaleB2 = 0.0;
  GetFactors(mDirR, mDirG, vDir, vScaleR1, vScaleG2);
  GetFactors(mDirG, mDirB, vDir, vScaleG1, vScaleB2);
  GetFactors(mDirB, mDirR, vDir, vScaleB1, vScaleR2);
  qreal vColorR = 1.0 + ggMin(vScaleR1, vScaleR2);
  qreal vColorG = 1.0 + ggMin(vScaleG1, vScaleG2);
  qreal vColorB = 1.0 + ggMin(vScaleB1, vScaleB2);
  return QColor::fromRgbF(ggClamp(vColorR, 0.0, 1.0),
                          ggClamp(vColorG, 0.0, 1.0),
                          ggClamp(vColorB, 0.0, 1.0),
                          mColorSaturized.alphaF());
}


QColor ggColorWheelWidget::GetColorFromWheel(const QPointF& aPosition) const
{
  QColor vColor = GetColorSaturized(aPosition);
  if (!mAdaptToValue) return vColor;
  return ggUtilityQt::GetColorScaled(vColor, mColorValue);
}


bool ggColorWheelWidget::IsInside(const QPointF& aPosition) const
{
  bool vIsInside = true;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirR, mDirY, aPosition) <= 0.0f;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirY, mDirG, aPosition) <= 0.0f;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirG, mDirC, aPosition) <= 0.0f;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirC, mDirB, aPosition) <= 0.0f;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirB, mDirM, aPosition) <= 0.0f;
  vIsInside = vIsInside && SignFromLine(mPosCenter, mDirM, mDirR, aPosition) <= 0.0f;
  return vIsInside;
}


bool ggColorWheelWidget::hasHeightForWidth() const
{
  return true;
}


int ggColorWheelWidget::heightForWidth(int aWidth) const
{
  const float vRatio = sqrt(3.0f) / 2.0f;
  return static_cast<int>(vRatio * aWidth);
}


void ggColorWheelWidget::mousePressEvent(QMouseEvent* aEvent)
{
  // change selected color
  if (aEvent->button() == Qt::LeftButton) {
    mMouseDragging = true;
    setCursor(Qt::BlankCursor);
    mColorSaturized = GetColorSaturized(aEvent->pos());
    mColorPosition = GetPosition(mColorSaturized);
    emit ColorChanged(GetColor());
    update();
  }

  // call base method
  QWidget::mousePressEvent(aEvent);
}


void ggColorWheelWidget::mouseReleaseEvent(QMouseEvent* aEvent)
{
  // change selected color
  if (aEvent->button() == Qt::LeftButton) {
    mMouseDragging = false;
    setCursor(IsInside(aEvent->pos()) ? Qt::CrossCursor : Qt::ArrowCursor);
    update();
  }

  // call base method
  QWidget::mouseReleaseEvent(aEvent);
}


void ggColorWheelWidget::mouseMoveEvent(QMouseEvent* aEvent)
{
  // adjust mouse pointer, if it is inside of the color-wheel
  if (mMouseDragging) {
    setCursor(IsInside(aEvent->pos()) ? Qt::BlankCursor : Qt::CrossCursor);
  }
  else {
    setCursor(IsInside(aEvent->pos()) ? Qt::CrossCursor : Qt::ArrowCursor);
  }

  // change selected color
  if (aEvent->buttons() & Qt::LeftButton) {
    mColorSaturized = GetColorSaturized(aEvent->pos());
    mColorPosition = GetPosition(mColorSaturized);
    emit ColorChanged(GetColor());
    update();
  }

  // call base function
  QWidget::mouseMoveEvent(aEvent);
}


void ggColorWheelWidget::resizeEvent(QResizeEvent* aEvent)
{
  // calculate half width, half height, and center
  float vSizeXH = aEvent->size().width() / 2.0f;
  float vSizeYH = aEvent->size().height() / 2.0f;
  float vPosCenterX = rect().x() + vSizeXH;
  float vPosCenterY = rect().y() + vSizeYH;

  // some space for the selector
  vSizeXH -= mSelectorRadius;
  vSizeYH -= mSelectorRadius;

  // fix aspect ratio with the height of an equilateral triangle
  const float vRatio = sqrt(3.0f) / 2.0f;
  vSizeXH = vSizeYH / vRatio < vSizeXH ? vSizeYH / vRatio : vSizeXH;
  vSizeYH = vSizeXH * vRatio < vSizeYH ? vSizeXH * vRatio : vSizeYH;

  // corner positions
  mPosCenter = QPointF(vPosCenterX, vPosCenterY);
  mDirR = QVector2D(      vSizeXH,     0.0f); // red corner
  mDirY = QVector2D( vSizeXH/2.0f, -vSizeYH); // yellow corner
  mDirG = QVector2D(-vSizeXH/2.0f, -vSizeYH); // green corner
  mDirC = QVector2D(     -vSizeXH,     0.0f); // cyan corner
  mDirB = QVector2D(-vSizeXH/2.0f,  vSizeYH); // blue corner
  mDirM = QVector2D( vSizeXH/2.0f,  vSizeYH); // magenta corner

  // position of the selected color
  mColorPosition = GetPosition(mColorSaturized);

  // call base class
  QWidget::resizeEvent(aEvent);
}


void DrawPolygon(QPainter& aPainter,
                 const QPointF aPosition,
                 const QVector2D& aDirectionA,
                 const QVector2D& aDirectionB,
                 const QVector2D& aDirectionC,
                 const QVector2D& aDirectionD,
                 const QVector2D& aDirectionE,
                 const QVector2D& aDirectionF)
{
  QPolygonF vPolygon;
  vPolygon << aPosition + aDirectionA.toPointF();
  vPolygon << aPosition + aDirectionB.toPointF();
  vPolygon << aPosition + aDirectionC.toPointF();
  vPolygon << aPosition + aDirectionD.toPointF();
  vPolygon << aPosition + aDirectionE.toPointF();
  vPolygon << aPosition + aDirectionF.toPointF();
  aPainter.drawPolygon(vPolygon);
}


void DrawRhomb(QPainter& aPainter,
               const QPointF& aPosition,
               const QVector2D& aDirectionA,
               const QVector2D& aDirectionB)
{
  QPolygonF vPolygon;
  vPolygon << aPosition;
  vPolygon << aPosition + aDirectionA.toPointF();
  vPolygon << aPosition + aDirectionA.toPointF() + aDirectionB.toPointF();
  vPolygon << aPosition + aDirectionB.toPointF();
  aPainter.drawPolygon(vPolygon);
}


QBrush GetGradientBrush(const QPointF& aPosition,
                        const QVector2D& aDirectionA,
                        const QVector2D& aDirectionB,
                        const QColor& aColorA,
                        const QColor& aColorB)
{
  // perpendicular direction (normal)
  QVector2D vDirectionAN(-aDirectionA.y(), aDirectionA.x());
  vDirectionAN.normalize();

  // project "B" perpendicular to "A"
  float vScaleAB = QVector2D::dotProduct(vDirectionAN, aDirectionB);
  QVector2D vDirectionBN(vScaleAB * vDirectionAN);

  // setup gradient
  QLinearGradient vGradient(aPosition, aPosition + vDirectionBN.toPointF());
  vGradient.setColorAt(0.0, aColorA);
  vGradient.setColorAt(1.0, aColorB);

  // return the gratent brush
  return QBrush(vGradient);
}


void DrawGradientRhomb(QPainter& aPainter,
                       const QPointF& aPosition,
                       const QVector2D& aDirectionA,
                       const QVector2D& aDirectionB,
                       const QColor& aColorA,
                       const QColor& aColorB,
                       const QColor& aColorC)
{
  // backup some painter settings
  QBrush vBrush(aPainter.brush());
  QPainter::CompositionMode vCompositionMode(aPainter.compositionMode());

  // paint rhomb  with default mode (paints background)
  aPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  aPainter.setBrush(GetGradientBrush(aPosition, aDirectionA, aDirectionB, aColorA, aColorC));
  DrawRhomb(aPainter, aPosition, aDirectionA, aDirectionB);

  // paint rhomb and use brighter color component (on top of background)
  aPainter.setCompositionMode(QPainter::CompositionMode_Lighten);
  aPainter.setBrush(GetGradientBrush(aPosition, aDirectionB, aDirectionA, aColorB, aColorC));
  DrawRhomb(aPainter, aPosition, aDirectionB, aDirectionA);

  // restore painter settings
  aPainter.setCompositionMode(vCompositionMode);
  aPainter.setBrush(vBrush);
}


static void DrawLine(QPainter& aPainter,
                     const QPointF& aPosition,
                     const QVector2D& aDirection,
                     qreal aOffset,
                     qreal aLength)
{
  QVector2D vDirection(aDirection.normalized());
  aPainter.drawLine(aPosition + (static_cast<float>(aOffset)*vDirection).toPointF(),
                    aPosition + (static_cast<float>(aOffset+aLength)*vDirection).toPointF());
}


void ggColorWheelWidget::paintEvent(QPaintEvent* aEvent)
{
  // painter
  QPainter vPainter(this);

  // draw corners, turn off antialiasing (there are artifacts)
  vPainter.setRenderHint(QPainter::Antialiasing, false);
  vPainter.setPen(Qt::NoPen);
  DrawGradientRhomb(vPainter, mPosCenter, mDirY, mDirM, mColorY, mColorM, mColorR); // red corner
  DrawGradientRhomb(vPainter, mPosCenter, mDirC, mDirY, mColorC, mColorY, mColorG); // green corner
  DrawGradientRhomb(vPainter, mPosCenter, mDirM, mDirC, mColorM, mColorC, mColorB); // blue corner

  // draw antialiased outline with window color (smooth the stairs)
  vPainter.setRenderHint(QPainter::Antialiasing, true);
  vPainter.setPen(palette().window().color());
  vPainter.setBrush(Qt::NoBrush);
  DrawPolygon(vPainter, mPosCenter, mDirR, mDirY, mDirG, mDirC, mDirB, mDirM);

  // indicator of selected color
  const qreal vSelectorRadiusLarge = 9.0;
  const qreal vRadius = mMouseDragging ? vSelectorRadiusLarge : mSelectorRadius;
  vPainter.setPen(QPen(Qt::white, 1.5));
  vPainter.drawEllipse(mColorPosition, vRadius + 1.0, vRadius + 1.0);
  vPainter.setPen(Qt::black);
  vPainter.setBrush(GetColor());
  vPainter.drawEllipse(mColorPosition, vRadius, vRadius);

  // center crosshair
  vPainter.setPen(ggUtilityQt::GetColorContrast(GetColorFromWheel(mPosCenter)));
  qreal vOffset = mSelectorRadius;
  qreal vLength = vSelectorRadiusLarge - mSelectorRadius - 2.5;
  DrawLine(vPainter, mPosCenter, mDirR, vOffset, vLength);
  DrawLine(vPainter, mPosCenter, mDirG, vOffset, vLength);
  DrawLine(vPainter, mPosCenter, mDirB, vOffset, vLength);

  // grey out, if disabled
  if (!isEnabled()) {
    vPainter.setPen(Qt::NoPen);
    const QColor& vColor = palette().color(QPalette::Disabled, QPalette::Window);
    vPainter.setBrush(ggUtilityQt::GetColorWithAlpha(vColor, 0.75));
    vPainter.drawRect(rect());
  }

  // maybe the parent wants to draw as well...
  QWidget::paintEvent(aEvent);
}
