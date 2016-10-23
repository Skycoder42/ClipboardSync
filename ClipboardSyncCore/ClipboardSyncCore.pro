#-------------------------------------------------
#
# Project created by QtCreator 2016-10-09T11:21:50
#
#-------------------------------------------------

QT       -= gui

TEMPLATE = lib

TARGET = ClipboardSyncCore
VERSION = 1.0.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "Clipboard-Sync Client"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
}

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
