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
    inputreader.cpp

HEADERS += clipboardsynccore_global.h \
    inputreader.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
