QT       += core gui widgets multimedia

INCLUDEPATH += $$PWD/src

CONFIG += console

TARGET = dmdwidget
TEMPLATE = app

SOURCES += $$PWD/main.cpp \
           $$PWD/mainwindow.cpp \
           $$PWD/dmdapplication.cpp \
           $$PWD/fx3process.cpp \
           $$PWD/dmdwidget.cpp \
           $$PWD/dmdwidget.cpp \
           $$PWD/dmdoutputdevice.cpp \
           $$PWD/widgetoutputdevice.cpp \
           $$PWD/pindmd2outputdevice.cpp \
           $$PWD/pin2dmdoutputdevice.cpp \
           $$PWD/dmdanimationengine.cpp \
           $$PWD/dmdanimation.cpp \
           $$PWD/animationthread.cpp \
           $$PWD/fx3animation.cpp \
           $$PWD/imageanimation.cpp \
           $$PWD/imageanimation.cpp \
           $$PWD/dmdframe.cpp \
           $$PWD/animationwindow.cpp \
           $$PWD/deviceconfigwindow.cpp \
           $$PWD/widgetoutputdeviceconfigtab.cpp \
           $$PWD/rominspectwindow.cpp \
           $$PWD/fantasieswindow.cpp \
           $$PWD/dmdeventfilter.cpp \
           $$PWD/dmdkeys.cpp \
           $$PWD/dmdconfig.cpp

HEADERS += $$PWD/mainwindow.h \
           $$PWD/dmdapplication.h \
		       $$PWD/fx3process.h \
           $$PWD/dmdwidget.h \
           $$PWD/dmdoutputdevice.h \
           $$PWD/widgetoutputdevice.h \
           $$PWD/pindmd2outputdevice.h \
           $$PWD/pin2dmdoutputdevice.h \
           $$PWD/dmdanimationengine.h \
           $$PWD/dmdanimation.h \
           $$PWD/animationthread.h \
           $$PWD/fx3animation.h \
           $$PWD/imageanimation.h \
           $$PWD/dmdframe.h \
           $$PWD/animationwindow.h \
           $$PWD/deviceconfigwindow.h \
           $$PWD/widgetoutputdeviceconfigtab.h \
           $$PWD/rominspectwindow.h \
           $$PWD/fantasieswindow.h \
           $$PWD/dmdeventfilter.h \
           $$PWD/dmdkeys.h \
           $$PWD/dmdconfig.h

RESOURCES += \
    dmdwidget.qrc

include("vendor/Qt-Color-Widgets/color_widgets.pri");

# Prevent lib/dll creation. We don't need that shit. Gives issues on win32
DEFINES += QTCOLORWIDGETS_STATICALLY_LINKED=1

INCLUDEPATH += "$$PWD/vendor/libusb"
INCLUDEPATH += "$$PWD/vendor/libusb-win32-bin-1.2.6.0/include"
LIBS += "$$PWD/vendor/libusb/x64/Release/lib/libusb-1.0.lib"
LIBS += "$$PWD/vendor/libusb-win32-bin-1.2.6.0/lib/msvc_x64/libusb.lib"

