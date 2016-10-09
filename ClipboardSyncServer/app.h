#ifndef APP_H
#define APP_H

#include <QCoreApplication>
#include "inputreader.h"
#include "syncserver.h"

class App : public QCoreApplication
{
	Q_OBJECT

public:
	explicit App(int &argc, char **argv);

	int exec();

private:
	InputReader *reader;
	SyncServer *syncServer;

	bool init(const QString &serverName, int port, bool secure, bool local);
};

#endif // APP_H
