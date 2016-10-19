#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStandardItemModel>
#include "toolmanager.h"
#include "menumanager.h"
#include "logdialog.h"
#include "peersdialog.h"

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
	void serverStatusLoaded(const QString &name, ToolManager::ServerInfo info);

	void showLog(const QString &name, const QByteArray &log);
	void showPeers(const QString &name, QStandardItemModel *peerModel);

private:
	QSystemTrayIcon *trayIco;
	ToolManager *toolManager;
	MenuManager *menuManager;

	QHash<QString, LogDialog*> logDialogs;
	QHash<QString, PeersDialog*> peersDialogs;

	void init();
};

#undef qApp
#define qApp ((App*)QApplication::instance())

#endif // APP_H
