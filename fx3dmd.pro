QT       += core gui widgets

INCLUDEPATH += $$PWD/src

CONFIG += console

TARGET = dmdwidget
TEMPLATE = app

SOURCES += $$PWD/main.cpp \
           $$PWD/mainwindow.cpp \
           $$PWD/fx3process.cpp \
           $$PWD/dmddata.cpp \
           $$PWD/dmdwidget.cpp \
           $$PWD/dmdwidget.cpp \
           $$PWD/dmdoutputdevice.cpp \
           $$PWD/widgetoutputdevice.cpp \
           $$PWD/pindmd2outputdevice.cpp \
           $$PWD/dmdanimation.cpp \
           $$PWD/dmdanimationframe.cpp

HEADERS += $$PWD/mainwindow.h \
		   $$PWD/fx3process.h \
		   $$PWD/dmddata.h \
           $$PWD/dmdwidget.h \
           $$PWD/dmdoutputdevice.h \
           $$PWD/widgetoutputdevice.h \
           $$PWD/pindmd2outputdevice.h \
           $$PWD/dmdanimation.h \
           $$PWD/dmdanimationframe.h
