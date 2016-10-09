QT += core websockets
QT -= gui

CONFIG += c++11

TARGET = ClipboardSyncServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ClipboardSyncCore/release/ -lClipboardSyncCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ClipboardSyncCore/debug/ -lClipboardSyncCore
else:unix: LIBS += -L$$OUT_PWD/../ClipboardSyncCore/ -lClipboardSyncCore

INCLUDEPATH += $$PWD/../ClipboardSyncCore
DEPENDPATH += $$PWD/../ClipboardSyncCore
