TEMPLATE = app

QT += core websockets
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = ClipboardSyncClient
VERSION = 1.0.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""

win32 {
	RC_ICONS += ../clipboard_sync.ico
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "Clipboard-Sync Client"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""
} else {
	DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"Clipboard-Sync Client\\\"\""
}

HEADERS += \
	app.h \
    syncclient.h

SOURCES += \
    app.cpp \
    syncclient.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ClipboardSyncCore/release/ -lClipboardSyncCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ClipboardSyncCore/debug/ -lClipboardSyncCore
else:unix: LIBS += -L$$OUT_PWD/../ClipboardSyncCore/ -lClipboardSyncCore

INCLUDEPATH += $$PWD/../ClipboardSyncCore
DEPENDPATH += $$PWD/../ClipboardSyncCore
