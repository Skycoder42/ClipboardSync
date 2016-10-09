#ifndef APP_H
#define APP_H

#include <QCoreApplication>
#include <QHostAddress>
#include "inputreader.h"
#include "syncclient.h"

class App : public QCoreApplication
{
	Q_OBJECT
public:
	explicit App(int &argc, char **argv);

	int exec();

private:
	InputReader *reader;
	SyncClient *client;

	bool init(const QString &address, const QString &serverName, bool secure);
};



#endif // APP_H
