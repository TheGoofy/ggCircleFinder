#ifndef GGGRAPHICSVIEW_H
#define GGGRAPHICSVIEW_H

#include <QGraphicsView>

class ggGraphicsWidget : public QGraphicsView
{

  Q_OBJECT

public:

  ggGraphicsWidget(QWidget *aParent = nullptr);
/*
  void itemMoved();

public slots:

  void shuffle();
  void zoomIn();
  void zoomOut();

protected:

  void keyPressEvent(QKeyEvent *event) override;
  void timerEvent(QTimerEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void drawBackground(QPainter *painter, const QRectF &rect) override;

  void scaleView(qreal scaleFactor);
*/
};

#endif // GGGRAPHICSVIEW_H
