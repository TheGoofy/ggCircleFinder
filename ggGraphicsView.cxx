#include "ggGraphicsView.h"

ggGraphicsWidget::ggGraphicsWidget(QWidget *parent)
  : QGraphicsView(parent)
{
  setBackgroundBrush(Qt::yellow);
  setRenderHint(QPainter::Antialiasing, false);
  setRenderHint(QPainter::SmoothPixmapTransform, false);
  QGraphicsScene* vScene = new QGraphicsScene(this);
  vScene->addPixmap(QPixmap(":/img/VerticalWindTurbine.png"));
  //vScene->addItem(QPen(Qt::green));
  vScene->addEllipse(0, 0, 1000, 500);
  vScene->addEllipse(200, 100, 500, 700);
//  vScene->addItem();
  setScene(vScene);
}
