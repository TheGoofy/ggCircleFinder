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
        main.cxx \
        MainWindow.cxx \
    ggRenderWidget.cxx \
    ggImage.cxx \
    ggImagePainterT.cxx

HEADERS += \
        MainWindow.h \
    ggRenderWidget.h \
    ggTypes.h \
    ggRunningAverages.h \
    ggLinear.h \
    ggVectorT.h \
    ggVector2T.h \
    ggVector3T.h \
    ggImageT.h \
    ggAssert.h \
    ggVectorTypes.h \
    ggImagePainterT.h \
    ggImageTypes.h \
    ggImageFilter.h \
    ggUtilities.h \
    ggHistogramAdaptiveT.h \
    ggGeometry.h \
    ggLineT.h \
    ggLineTypes.h \
    ggSpotT.h \
    ggSpotTypes.h

FORMS += \
        MainWindow.ui

RESOURCES +=
