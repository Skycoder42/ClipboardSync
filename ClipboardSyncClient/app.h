#ifndef APP_H
#define APP_H

#include <QGuiApplication>
#include <QHostAddress>
#include <QLockFile>
#include <QScopedPointer>
#include "console.h"
#include "syncclient.h"
#include "clipboardcontroller.h"

class App : public QGuiApplication
{
	Q_OBJECT
public:
	explicit App(int &argc, char **argv);

	int exec();

private slots:
	void commandReceived(const QByteArray &command);

private:
	Console *console;
	QScopedPointer<QLockFile> serverLock;
	SyncClient *client;
	ClipboardController *clipController;

	bool gainLock(const QString &host);
};



#endif // APP_H
