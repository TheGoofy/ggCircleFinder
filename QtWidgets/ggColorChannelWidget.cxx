// 0) include own header
#include "ggColorChannelWidget.h"

// 1) include system or QT
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QBitmap>
#include <QLayout>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggUtility.h"
#include "QtBase/ggUtilityQt.h"


ggColorChannelWidget::ggColorChannelWidget(QWidget* aParent) :
  QWidget(aParent),
  mChannel(cChannel::eValue),
  mSelectorRadius(3.0),
  mSelectorRadiusLarge(9.0),
  mMouseDragging(false),
  mLayout(cLayout::eVertical)
{
  setMouseTracking(true);
  SetHorizontal();
  SetColor(QColor(200, 150, 50, 255));
}


void ggColorChannelWidget::SetColor(const QColor& aColor)
{
  // update colors, if changed
  if (aColor != mColor) {
    mColor = aColor;
    mChannelValue = GetChannelValue(mColor);
    mColorPosition = GetPosition(mChannelValue);
    update(); // trigger repaint
  }
}


void ggColorChannelWidget::SetChannel(cChannel aChannel)
{
  if (aChannel != mChannel) {
    mChannel = aChannel;
    mChannelValue = GetChannelValue(mColor);
    mColorPosition = GetPosition(mChannelValue);
    update(); // trigger repaint
  }
}


ggColorChannelWidget::cChannel ggColorChannelWidget::GetChannel() const
{
  return mChannel;
}


void ggColorChannelWidget::SetHorizontal()
{
  if (mLayout != cLayout::eHorizontal) {
    mLayout = cLayout::eHorizontal;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    update(); // trigger repaint
  }
}


void ggColorChannelWidget::SetVertical()
{
  if (mLayout != cLayout::eVertical) {
    mLayout = cLayout::eVertical;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    update(); // trigger repaint
  }
}


bool ggColorChannelWidget::IsHorizontal() const
{
  return mLayout == cLayout::eHorizontal;
}


bool ggColorChannelWidget::IsVertical() const
{
  return mLayout == cLayout::eVertical;
}


qreal ggColorChannelWidget::GetChannelValue(const QColor& aColor) const
{
  switch (mChannel) {
    case cChannel::eRed: return aColor.redF();
    case cChannel::eGreen: return aColor.greenF();
    case cChannel::eBlue: return aColor.blueF();
    case cChannel::eAlpha: return aColor.alphaF();
    case cChannel::eValue: return aColor.valueF();
    case cChannel::eLightness: return aColor.lightnessF();
    default: return 0.0;
  }
}


QColor ggColorChannelWidget::GetColor() const
{
  QColor vColor(mColor);
  switch (mChannel) {
    case cChannel::eRed: vColor.setRedF(mChannelValue); break;
    case cChannel::eGreen: vColor.setGreenF(mChannelValue); break;
    case cChannel::eBlue: vColor.setBlueF(mChannelValue); break;
    case cChannel::eAlpha: vColor.setAlphaF(mChannelValue); break;
    case cChannel::eValue: vColor = ggUtilityQt::GetColorWithValue(mColor, mChannelValue); break;
    case cChannel::eLightness: vColor = ggUtilityQt::GetColorWithLightness(vColor, mChannelValue); break;
    default: break;
  }
  return vColor;
}


QColor ggColorChannelWidget::GetColorMax() const
{
  switch (mChannel) {
    case cChannel::eRed: return QColor(Qt::red);
    case cChannel::eGreen: return QColor(Qt::green);
    case cChannel::eBlue: return QColor(Qt::blue);
    case cChannel::eAlpha: return ggUtilityQt::GetColorWithAlpha(mColor, 1.0);
    case cChannel::eValue: return ggUtilityQt::GetColorWithAlpha(ggUtilityQt::GetColorSaturized(mColor), 1.0);
    case cChannel::eLightness: return QColor(Qt::white);
    default: return mColor;
  }
}


QColor ggColorChannelWidget::GetColorMid() const
{
  if (mChannel == cChannel::eLightness) {
    QColor vColor = ggUtilityQt::GetColorWithLightness(mColor, 0.5);
    vColor.setAlphaF(1.0);
    return vColor;
  }
  else {
    return ggUtilityQt::GetColorInterpolated(GetColorMin(), GetColorMax(), 0.5);
  }
}


QColor ggColorChannelWidget::GetColorMin() const
{
  switch (mChannel) {
    case cChannel::eRed: return QColor(Qt::black);
    case cChannel::eGreen: return QColor(Qt::black);
    case cChannel::eBlue: return QColor(Qt::black);
    case cChannel::eAlpha: return ggUtilityQt::GetColorWithAlpha(mColor, 0.0);
    case cChannel::eValue: return QColor(Qt::black);
    case cChannel::eLightness: return QColor(Qt::black);
    default: return QColor(Qt::black);
  }
}


QPointF ggColorChannelWidget::ClampPosition(const QPointF& aPosition) const
{
  return QPointF(ggUtility::Clamp(aPosition.x(), mColorBar.x(), mColorBar.right()),
                 ggUtility::Clamp(aPosition.y(), mColorBar.y(), mColorBar.bottom()));
}


QPointF ggColorChannelWidget::GetPosition(const qreal aChannelValue) const
{
  if (mLayout == cLayout::eHorizontal) {
    return QPointF(mColorBar.x() + aChannelValue * mColorBar.width(), mColorBar.center().y());
  }
  if (mLayout == cLayout::eVertical) {
    return QPointF(mColorBar.center().x(), mColorBar.bottom() - aChannelValue * mColorBar.height());
  }
  return mColorBar.center();
}


qreal ggColorChannelWidget::GetChannelValue(const QPointF& aPosition) const
{
  QPointF vPosition = ClampPosition(aPosition);
  if (mLayout == cLayout::eHorizontal) {
    return (vPosition.x() - mColorBar.x()) / mColorBar.width();
  }
  if (mLayout == cLayout::eVertical) {
    return (mColorBar.bottom() - vPosition.y()) / mColorBar.height();
  }
  return 0.5;
}


bool ggColorChannelWidget::IsInside(const QPointF& aPosition) const
{
  return mColorBar.contains(aPosition);
}


QSize ggColorChannelWidget::sizeHint() const
{
  int vSize = static_cast<int>(2.0 * mSelectorRadiusLarge);
  QSize vSizeHint(vSize, vSize);
  if (IsHorizontal()) vSizeHint += QSize(vSize, 0);
  if (IsVertical()) vSizeHint += QSize(0, vSize);
  return vSizeHint;
}


void ggColorChannelWidget::mousePressEvent(QMouseEvent* aEvent)
{
  // change selected color
  if (aEvent->button() == Qt::LeftButton) {
    mMouseDragging = true;
    setCursor(Qt::BlankCursor);
    mChannelValue = GetChannelValue(aEvent->pos());
    mColorPosition = GetPosition(mChannelValue);
    emit ColorChanged(GetColor());
    update();
  }

  // base method
  QWidget::mousePressEvent(aEvent);
}


void ggColorChannelWidget::mouseReleaseEvent(QMouseEvent* aEvent)
{
  // change selected color
  if (aEvent->button() == Qt::LeftButton) {
    mMouseDragging = false;
    setCursor(IsInside(aEvent->pos()) ? Qt::CrossCursor : Qt::ArrowCursor);
    update();
  }

  // base method
  QWidget::mouseReleaseEvent(aEvent);
}


void ggColorChannelWidget::mouseMoveEvent(QMouseEvent* aEvent)
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
    mChannelValue = GetChannelValue(aEvent->pos());
    mColorPosition = GetPosition(mChannelValue);
    emit ColorChanged(GetColor());
    update();
  }

  // base method
  QWidget::mouseMoveEvent(aEvent);
}


void ggColorChannelWidget::resizeEvent(QResizeEvent* aEvent)
{
  // color bar corners
  mColorBar = QRectF(mSelectorRadius, mSelectorRadius,
                     aEvent->size().width() - 2.0 * mSelectorRadius,
                     aEvent->size().height() - 2.0 * mSelectorRadius);

  // position of the selected color
  mColorPosition = GetPosition(mChannelValue);

  // base method
  QWidget::resizeEvent(aEvent);
}


QBrush GradientBrush(const QPointF& aPositionA,
                     const QPointF& aPositionB,
                     const QColor& aColorA,
                     const QColor& aColorB)
{
  // setup gradient
  QLinearGradient vGradient(aPositionA, aPositionB);
  vGradient.setColorAt(0.0, aColorA);
  vGradient.setColorAt(1.0, aColorB);

  // return the gratent brush
  return QBrush(vGradient);
}


QBrush GradientBrush(const QPointF& aPositionA,
                     const QPointF& aPositionB,
                     const QColor& aColorA,
                     const QColor& aColorB,
                     const QColor& aColorC)
{
  // setup gradient
  QLinearGradient vGradient(aPositionA, aPositionB);
  vGradient.setColorAt(0.0, aColorA);
  vGradient.setColorAt(0.5, aColorB);
  vGradient.setColorAt(1.0, aColorC);

  // return the gratent brush
  return QBrush(vGradient);
}


QBrush ggColorChannelWidget::GetGradientBrush() const
{
  if (mLayout == cLayout::eHorizontal) {
    return ::GradientBrush(mColorBar.topLeft(), mColorBar.topRight(),
                           GetColorMin(), GetColorMid(), GetColorMax());
  }
  if (mLayout == cLayout::eVertical) {
    return ::GradientBrush(mColorBar.bottomLeft(), mColorBar.topLeft(),
                           GetColorMin(), GetColorMid(), GetColorMax());
  }
  return QBrush(mColor);
}


QRectF ggColorChannelWidget::GetSelectorRect(qreal aSize) const
{
  qreal vSize2 = aSize / 2.0;
  if (mLayout == cLayout::eHorizontal) {
    return QRectF(mColorPosition.x() - vSize2, mColorBar.top() - mSelectorRadius + 1.0,
                  aSize, mColorBar.height() + 2.0 * mSelectorRadius - 2.0);
  }
  if (mLayout == cLayout::eVertical) {
    return QRectF(mColorBar.x() - mSelectorRadius + 1.0, mColorPosition.y() - vSize2,
                  mColorBar.width() + 2.0 * mSelectorRadius - 2.0, aSize);
  }
  return QRectF(mColorPosition.x() - vSize2, mColorPosition.y() - vSize2,
                aSize, aSize);
}


void ggColorChannelWidget::paintEvent(QPaintEvent* aEvent)
{
  // the friendly painter
  QPainter vPainter(this);
  vPainter.setRenderHint(QPainter::Antialiasing, true);

  // draw the bar
  vPainter.setPen(Qt::NoPen);
  if (mChannel == cChannel::eAlpha) {
    vPainter.setBrush(QColor(130,130,130,255));
    vPainter.drawRoundedRect(mColorBar, 2.0 * mSelectorRadius, 2.0 * mSelectorRadius);
    vPainter.setBrush(ggUtilityQt::GetCheckerBoardBrush(static_cast<int>(1.5 * mSelectorRadius)));
    vPainter.drawRoundedRect(mColorBar, 2.0 * mSelectorRadius, 2.0 * mSelectorRadius);
  }
  vPainter.setBrush(GetGradientBrush());
  vPainter.drawRoundedRect(mColorBar, 2.0 * mSelectorRadius, 2.0 * mSelectorRadius);

  // indicator of selected color
  qreal vRadius = mMouseDragging ? mSelectorRadiusLarge : mSelectorRadius;
  vPainter.setPen(QPen(Qt::white, 1.5));
  vPainter.setBrush(Qt::NoBrush);
  vPainter.drawEllipse(mColorPosition, vRadius + 1.0, vRadius + 1.0);
  vPainter.setPen(Qt::black);
  vPainter.setBrush(GetColor());
  vPainter.drawEllipse(mColorPosition, vRadius, vRadius);

  // center indicator
  vPainter.setPen(Qt::black);
  qreal vOffset = mSelectorRadius;
  qreal vLength = mSelectorRadiusLarge - mSelectorRadius - 2.5;
  const QPointF& vCenter = mColorBar.center();
  if (IsHorizontal()) {
    vPainter.drawLine(QPointF(vCenter.x(), vCenter.y() - vOffset), QPointF(vCenter.x(), vCenter.y() - vOffset - vLength));
    vPainter.drawLine(QPointF(vCenter.x(), vCenter.y() + vOffset), QPointF(vCenter.x(), vCenter.y() + vOffset + vLength));
  }
  if (IsVertical()) {
    vPainter.drawLine(QPointF(vCenter.x() - vOffset, vCenter.y()), QPointF(vCenter.x() - vOffset - vLength, vCenter.y()));
    vPainter.drawLine(QPointF(vCenter.x() + vOffset, vCenter.y()), QPointF(vCenter.x() + vOffset + vLength, vCenter.y()));
  }

  // grey out, if disabled
  if (!isEnabled()) {
    vPainter.setPen(Qt::NoPen);
    const QColor& vColor = palette().color(QPalette::Disabled, QPalette::Window);
    vPainter.setBrush(ggUtilityQt::GetColorWithAlpha(vColor, 0.75));
    vPainter.drawRect(rect());
  }

  // base paint event
  QWidget::paintEvent(aEvent);
}
