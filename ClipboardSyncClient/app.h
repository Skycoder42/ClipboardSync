#ifndef APP_H
#define APP_H

#include <QCoreApplication>
#include <QHostAddress>
#include "console.h"
#include "syncclient.h"

class App : public QCoreApplication
{
	Q_OBJECT
public:
	explicit App(int &argc, char **argv);

	int exec();

private:
	Console *console;
	SyncClient *client;
};



#endif // APP_H
