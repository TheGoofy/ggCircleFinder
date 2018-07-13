#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Base/ggObserver.h"

namespace Ui {
  class MainWindow;
}

class MainWindow :
  public QMainWindow,
  public ggObserver
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *aParent = 0);
  ~MainWindow();

  Ui::MainWindow* GetUI();

protected slots:
  void on_mZoomComboBox_activated(int aIndex);
  void on_mZoomComboBox_editingFinished();
  void on_mZoomFitPushButton_clicked();
  void on_mZoomResetPushButton_clicked();

protected:
  void UpdateUI();
  void Update(const ggSubject* aSubject);

private:
  QString ZoomToString(float aZoomFloat) const;
  float StringToZoom(const QString& aZoomString) const;
  Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
