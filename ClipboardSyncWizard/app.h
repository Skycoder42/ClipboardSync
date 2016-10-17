#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include "toolmanager.h"
#include "menumanager.h"

class App : public QApplication
{
	Q_OBJECT

public:
	explicit App(int &argc, char **argv);

	int exec();

	bool testNameUnique(const QString &name) const;

public slots:
	void showCreate();

private slots:
	void showMessage(QtMsgType type, const QString &title, const QString &message);

private:
	QSystemTrayIcon *trayIco;
	ToolManager *toolManager;
	MenuManager *menuManager;

	void init();
};

#undef qApp
#define qApp ((App*)QApplication::instance())

#endif // APP_H
