#-------------------------------------------------
#
# Среда полуэмпирического моделирования
# распространения COVID-19
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = EpiCalc
CONFIG   += app_bundle

CONFIG   += qwt

TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    graphsdialog.cpp \
    countermeasure.cpp \
    episim.cpp

HEADERS += \
    mainwindow.h \
    graphsdialog.h \
    countermeasure.h \
    episim.h

FORMS += \
    mainwindow.ui \
    graphsdialog.ui \
    countermeasure.ui

#INCLUDEPATH += ../../qwt-6.1.3/qwt-6.1.3/src
#DEPENDPATH  += ../../qwt-6.1.3/qwt-6.1.3/src
#LIBS += -L../../qwt-6.1.3/qwt-6.1.3/lib -lqwt

RESOURCES += \
    resources.qrc
