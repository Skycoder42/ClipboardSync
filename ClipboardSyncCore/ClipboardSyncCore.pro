#-------------------------------------------------
#
# Project created by QtCreator 2016-10-09T11:21:50
#
#-------------------------------------------------

QT       -= gui

TARGET = ClipboardSyncCore
TEMPLATE = lib

DEFINES += CLIPBOARDSYNCCORE_LIBRARY

SOURCES += \
    console.cpp

HEADERS += clipboardsynccore_global.h \
    console.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
