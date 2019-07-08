QT       += core gui widgets

INCLUDEPATH += $$PWD/src

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
           $$PWD/pindmd2outputdevice.cpp

HEADERS += $$PWD/mainwindow.h \
		   $$PWD/fx3process.h \
		   $$PWD/dmddata.h \
           $$PWD/dmdwidget.h \
           $$PWD/dmdoutputdevice.h \
           $$PWD/widgetoutputdevice.h \
           $$PWD/pindmd2outputdevice.h
