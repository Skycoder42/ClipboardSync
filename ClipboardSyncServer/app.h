#ifndef APP_H
#define APP_H

#include <QCoreApplication>
#include "console.h"
#include "syncserver.h"

class App : public QCoreApplication
{
	Q_OBJECT

public:
	explicit App(int &argc, char **argv);

	int exec();

private:
	Console *console;
	SyncServer *syncServer;

	bool init(const QString &serverName, int port, bool secure, const QString &password, bool local);
};

#endif // APP_H
