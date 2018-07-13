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
    Base/ggItemLinked.cxx \
    Base/ggObserver.cxx \
    Base/ggSubject.cxx \
    Base/ggUtility.cxx \
    BaseGraphicsQT/ggConnectionPoint.cxx \
    BaseGraphicsQT/ggDecoration.cxx \
    BaseGraphicsQT/ggGraphicsAutoConnectPathItem.cxx \
    BaseGraphicsQT/ggGraphicsCheckBoxItem.cxx \
    BaseGraphicsQT/ggGraphicsConnectionPointItem.cxx \
    BaseGraphicsQT/ggGraphicsDecoratedPathItem.cxx \
    BaseGraphicsQT/ggGraphicsRoundedRectItem.cxx \
    BaseGraphicsQT/ggGraphicsShadowRectItem.cxx \
    BaseGraphicsQT/ggGraphicsTextItem.cxx \
    BaseGraphicsQT/ggGraphicsView.cxx \
    BaseGraphicsQT/ggPainterPath.cxx \
    BaseGraphicsQT/ggSubjectConnectionPoint.cxx \
    BaseImage/ggImage.cxx \
    BaseImage/ggImagePainterT.cxx \
    Main/ggRenderWidget.cxx \
    Main/main.cxx \
    Main/MainWindow.cxx

HEADERS += \
    Base/ggAssert.h \
    Base/ggBehaviorBlocking.h \
    Base/ggBehaviorLazy.h \
    Base/ggBehaviorT.h \
    Base/ggGeometry.h \
    Base/ggItemLinked.h \
    Base/ggLinear.h \
    Base/ggLineT.h \
    Base/ggLineTypes.h \
    Base/ggObserver.h \
    Base/ggRunningAverages.h \
    Base/ggSpotT.h \
    Base/ggSpotTypes.h \
    Base/ggString.h \
    Base/ggSubject.h \
    Base/ggSubjectT.h \
    Base/ggSubjectValueT.h \
    Base/ggTypes.h \
    Base/ggUtility.h \
    Base/ggVector2T.h \
    Base/ggVector3T.h \
    Base/ggVectorSetT.h \
    Base/ggVectorT.h \
    Base/ggVectorTypes.h \
    Base/ggWalkerT.h \
    BaseGraphicsQT/ggConnectionPoint.h \
    BaseGraphicsQT/ggDecoration.h \
    BaseGraphicsQT/ggGraphicsAutoConnectPathItem.h \
    BaseGraphicsQT/ggGraphicsCheckBoxItem.h \
    BaseGraphicsQT/ggGraphicsConnectionPointItem.h \
    BaseGraphicsQT/ggGraphicsDecoratedPathItem.h \
    BaseGraphicsQT/ggGraphicsHandleItemT.h \
    BaseGraphicsQT/ggGraphicsManipulatorBarItemT.h \
    BaseGraphicsQT/ggGraphicsManipulatorRectItemT.h \
    BaseGraphicsQT/ggGraphicsRoundedRectItem.h \
    BaseGraphicsQT/ggGraphicsShadowRectItem.h \
    BaseGraphicsQT/ggGraphicsTextItem.h \
    BaseGraphicsQT/ggGraphicsView.h \
    BaseGraphicsQT/ggPainterPath.h \
    BaseGraphicsQT/ggSubjectConnectionPoint.h \
    BaseImage/ggHistogramAdaptiveT.h \
    BaseImage/ggImageFilter.h \
    BaseImage/ggImagePainterT.h \
    BaseImage/ggImageT.h \
    BaseImage/ggImageTypes.h \
    Main/ggRenderWidget.h \
    Main/MainWindow.h

FORMS += \
    Main/MainWindow.ui

RESOURCES +=
