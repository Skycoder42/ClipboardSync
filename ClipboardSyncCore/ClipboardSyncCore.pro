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
    console.cpp \
    synccore.cpp

HEADERS += clipboardsynccore_global.h \
    console.h \
    synccore.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
