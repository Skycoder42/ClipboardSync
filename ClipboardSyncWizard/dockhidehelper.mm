#include "dockhidehelper.h"
#include <QEvent>
#include <QApplication>
#include <QWidget>
#include <QIcon>
#import <Cocoa/Cocoa.h>

DockHideHelper::DockHideHelper(QObject *parent) :
    QObject(parent),
    isHidden(false)
{
    this->setDockIconStyle(true);

    qApp->installEventFilter(this);
    connect(qApp, &QApplication::lastWindowClosed, this, [this](){
        this->setDockIconStyle(true);
    });
}

bool DockHideHelper::eventFilter(QObject *, QEvent *event)
{
    if(event->type() == QEvent::Show) {
        auto visible = false;
        foreach(auto widget, QApplication::topLevelWidgets()) {
            if(widget->isVisible())
                visible = true;
        }
        if(visible)
            this->setDockIconStyle(false);
    }

    return false;
}

void DockHideHelper::setDockIconStyle(bool hidden)
{
    if(hidden == this->isHidden)
        return;
    this->isHidden = hidden;

    ProcessSerialNumber psn = { 0, kCurrentProcess };
    OSStatus err;
    if (hidden)
        err = TransformProcessType(&psn, kProcessTransformToUIElementApplication);
    else {
        err = TransformProcessType(&psn, kProcessTransformToForegroundApplication);
        auto icon = QApplication::windowIcon();
        QApplication::setWindowIcon(QIcon());
        QApplication::setWindowIcon(icon);
    }
    if (err != noErr)
        qWarning("setDockIconStyle %s failure, status code: %d\n", (hidden ? "hidden" : "show"), err);
}
