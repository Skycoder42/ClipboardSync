#-------------------------------------------------
#
# Project created by QtCreator 2016-10-08T18:39:48
#
#-------------------------------------------------

TEMPLATE = app
QT       += core gui widgets network

CONFIG += c++11

TARGET = ClipboardSyncWizard
VERSION = 1.0.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""

win32 {
	RC_ICONS += ../clipboard_sync.ico
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "Clipboard-Sync"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""
} else {
	DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"Clipboard-Sync\\\"\""
}

include(../QPathEdit/qpathedit.pri)
include(../QtUtils/DialogMaster/dialogmaster.pri)

HEADERS  += \
	intropage.h \
	mainwizard.h \
	serversetuppage.h \
	clientsetuppage.h \
    finalpage.h \
    toolmanager.h \
    app.h \
    menumanager.h \
    logdialog.h \
	peersdialog.h

SOURCES += intropage.cpp \
    mainwizard.cpp \
    serversetuppage.cpp \
	clientsetuppage.cpp \
    finalpage.cpp \
    toolmanager.cpp \
    app.cpp \
    menumanager.cpp \
    logdialog.cpp \
	peersdialog.cpp

FORMS    += \
    intropage.ui \
    serversetuppage.ui \
    clientsetuppage.ui \
    finalpage.ui \
    logdialog.ui \
    peersdialog.ui

RESOURCES += \
	../sync_global_res.qrc

mac {
	HEADERS +=  \
		dockhidehelper.h

	OBJECTIVE_SOURCES += \
		dockhidehelper.mm

	LIBS += -framework AppKit
}
