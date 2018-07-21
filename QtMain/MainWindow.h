#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma GCC diagnostic ignored "-Wdouble-promotion"

#include <QMainWindow>

#include "LibBase/ggTypes.h"
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

  explicit MainWindow(QWidget *aParent = nullptr);
  virtual ~MainWindow() override;

  Ui::MainWindow& UI();

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
  void on_mCirclesOpacitySlider_valueChanged(int aValue);

protected:

  virtual void Update(const ggSubject* aSubject) override;
  virtual void closeEvent(QCloseEvent* aEvent) override;

private:

  void UpdateROI();
  void UpdateUserInterfaceZoom();
  void UpdateUserInterfaceROI();
  void WriteSettings();
  void ReadSettings();
  QString ZoomToString(ggDouble aZoomFloat) const;
  ggDouble StringToZoom(const QString& aZoomString) const;
  
  Ui::MainWindow* mUserInterface;
  
  ggGraphicsScene* mScene;

};

#endif // MAINWINDOW_H
