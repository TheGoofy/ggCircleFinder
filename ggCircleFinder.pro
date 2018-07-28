#-------------------------------------------------
#
# Project created by QtCreator 2017-09-30T14:49:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ggCircleFinder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    LibBase/ggItemLinked.cxx \
    LibBase/ggObserver.cxx \
    LibBase/ggSubject.cxx \
    LibBase/ggUtility.cxx \
    LibImage/ggImage.cxx \
    LibImage/ggImagePainterT.cxx \
    QtMain/main.cxx \
    QtMain/MainWindow.cxx \
    QtBase/ggUtilityQt.cxx \
    QtGraphics/ggConnectionPoint.cxx \
    QtGraphics/ggDecoration.cxx \
    QtGraphics/ggGraphicsAutoConnectPathItem.cxx \
    QtGraphics/ggGraphicsCheckBoxItem.cxx \
    QtGraphics/ggGraphicsConnectionPointItem.cxx \
    QtGraphics/ggGraphicsDecoratedPathItem.cxx \
    QtGraphics/ggGraphicsRoundedRectItem.cxx \
    QtGraphics/ggGraphicsShadowRectItem.cxx \
    QtGraphics/ggGraphicsTextItem.cxx \
    QtGraphics/ggGraphicsView.cxx \
    QtGraphics/ggPainterPath.cxx \
    QtGraphics/ggSubjectConnectionPoint.cxx \
    QtWidgets/ggColorChannelWidget.cxx \
    QtWidgets/ggColorDialog.cxx \
    QtWidgets/ggColorPreviewWidget.cxx \
    QtWidgets/ggColorWheelWidget.cxx \
    QtMain/ggGraphicsScene.cxx \
    LibImage/ggImageFilter.cxx \
    LibBase/ggRound.cxx \
    LibBase/ggRoundTest.cxx

HEADERS += \
    LibBase/ggAssert.h \
    LibBase/ggBehaviorBlocking.h \
    LibBase/ggBehaviorLazy.h \
    LibBase/ggBehaviorT.h \
    LibBase/ggColorT.h \
    LibBase/ggColorTypes.h \
    LibBase/ggGeometry.h \
    LibBase/ggItemLinked.h \
    LibBase/ggLinear.h \
    LibBase/ggLineT.h \
    LibBase/ggLineTypes.h \
    LibBase/ggObserver.h \
    LibBase/ggRunningAverages.h \
    LibBase/ggSpotT.h \
    LibBase/ggSpotTypes.h \
    LibBase/ggString.h \
    LibBase/ggSubject.h \
    LibBase/ggSubjectT.h \
    LibBase/ggSubjectValueT.h \
    LibBase/ggUtility.h \
    LibBase/ggVector2T.h \
    LibBase/ggVector3T.h \
    LibBase/ggVectorSetT.h \
    LibBase/ggVectorT.h \
    LibBase/ggVectorTypes.h \
    LibBase/ggWalkerT.h \
    LibImage/ggHistogramAdaptiveT.h \
    LibImage/ggImageFilter.h \
    LibImage/ggImagePainterT.h \
    LibImage/ggImageT.h \
    LibImage/ggImageTypes.h \
    QtMain/MainWindow.h \
    QtBase/ggUtilityQt.h \
    QtGraphics/ggConnectionPoint.h \
    QtGraphics/ggDecoration.h \
    QtGraphics/ggGraphicsAutoConnectPathItem.h \
    QtGraphics/ggGraphicsCheckBoxItem.h \
    QtGraphics/ggGraphicsConnectionPointItem.h \
    QtGraphics/ggGraphicsDecoratedPathItem.h \
    QtGraphics/ggGraphicsHandleItemT.h \
    QtGraphics/ggGraphicsManipulatorBarItemT.h \
    QtGraphics/ggGraphicsManipulatorRectItemT.h \
    QtGraphics/ggGraphicsRoundedRectItem.h \
    QtGraphics/ggGraphicsShadowRectItem.h \
    QtGraphics/ggGraphicsTextItem.h \
    QtGraphics/ggGraphicsView.h \
    QtGraphics/ggPainterPath.h \
    QtGraphics/ggSubjectConnectionPoint.h \
    QtWidgets/ggColorChannelWidget.h \
    QtWidgets/ggColorDialog.h \
    QtWidgets/ggColorPreviewWidget.h \
    QtWidgets/ggColorWheelWidget.h \
    LibImage/ggImageAlgorithm.h \
    QtMain/ggGraphicsScene.h \
    LibBase/ggFilterT.h \
    LibImage/ggImageLabeling.h \
    LibBase/ggRound.h \
    LibBase/ggNumberTypes.h \
    LibBase/ggNumerics.h \
    LibBase/ggRoundTest.h

FORMS += \
    QtMain/MainWindow.ui \
    QtWidgets/ggColorDialog.ui

RESOURCES +=
