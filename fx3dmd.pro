QT       += core gui widgets

INCLUDEPATH += $$PWD/src

TARGET = dmdwidget
TEMPLATE = app

SOURCES += $$PWD/main.cpp \
           $$PWD/mainwindow.cpp \
           $$PWD/dmdwidget.cpp

HEADERS += $$PWD/mainwindow.h \
           $$PWD/dmdwidget.h