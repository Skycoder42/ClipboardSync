#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include "toolmanager.h"

class App : public QApplication
{
	Q_OBJECT

public:
	explicit App(int &argc, char **argv);
	~App();

	int exec();

private slots:
	void showCreate();
	void errorOccured(bool isServer, const QString &name, const QString &error);

private:
	QSystemTrayIcon *trayIco;
	QMenu *trayMenu;

	ToolManager *manager;

	void init();
};

#endif // APP_H
