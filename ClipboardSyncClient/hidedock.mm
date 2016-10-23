#include <QCoreApplication>
#import <Cocoa/Cocoa.h>

static void doHide() {
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
}

Q_COREAPP_STARTUP_FUNCTION(doHide)
