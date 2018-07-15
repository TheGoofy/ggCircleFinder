#ifndef GGRENDERWIDGET_H
#define GGRENDERWIDGET_H

#include <QFrame>
#include <QPixmap>
#include <QGraphicsWidget>

#include "LibImage/ggImageT.h"


class ggRenderWidget : public QFrame
{
  Q_OBJECT

public:
  explicit ggRenderWidget(QWidget *aParent = nullptr);
  void SetPixmap(const QPixmap& aPixmap);

signals:

public slots:

private slots:
  void LoadImage();
  void GenerateImage();
  void FindCircles();

protected:
  void paintEvent(QPaintEvent* aEvent);// override;

private:
  void PrepareCameraImage(const ggImageT<ggFloat>& aImageFloat);

  ggImageT<ggUInt16> mImageCamera;
  QPixmap mImagePixmap;
  QPixmap mOverlayPixmap;
};

#endif // GGRENDERWIDGET_H
