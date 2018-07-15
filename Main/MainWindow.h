#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "LibBase/ggObserver.h"

namespace Ui {
  class MainWindow;
}

class ggGraphicsScene;

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

  // zoom (status bar)
  void on_mZoomComboBox_activated(int aIndex);
  void on_mZoomComboBox_editingFinished();
  void on_mZoomFitPushButton_clicked();
  void on_mZoomResetPushButton_clicked();

  // ROI
  void on_mRegionOfInterestPositionXLineEdit_editingFinished();
  void on_mRegionOfInterestPositionYLineEdit_editingFinished();
  void on_mRegionOfInterestSizeXLineEdit_editingFinished();
  void on_mRegionOfInterestSizeYLineEdit_editingFinished();

  //
  void on_mGenerateImagePushButton_clicked();
  void on_mLoadImagePushButton_clicked();
  void on_mFindCirclesPushButton_clicked();

  void on_mOverlayOpacitySlider_valueChanged(int aValue);

protected:

  virtual void Update(const ggSubject* aSubject) override;
  virtual void closeEvent(QCloseEvent* aEvent) override;

private:

  void UpdateROI();
  void UpdateUserInterfaceZoom();
  void UpdateUserInterfaceROI();
  void WriteSettings();
  void ReadSettings();
  QString ZoomToString(float aZoomFloat) const;
  float StringToZoom(const QString& aZoomString) const;
  
  Ui::MainWindow* ui;
  
  ggGraphicsScene* mScene;

};

#endif // MAINWINDOW_H
