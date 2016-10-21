#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QScopedPointer>
#include <QAction>
#include <QPointer>
#include "toolmanager.h"

class MenuManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool createEnabled READ isCreateEnabled WRITE setCreateEnabled NOTIFY createEnabledChanged)

public:
	explicit MenuManager(QSystemTrayIcon *trayIco, QObject *parent = nullptr);

	bool isCreateEnabled() const;

public slots:
	void addServer(const QString &name, bool fromStartup);
	void addClient(const QString &name, bool fromStartup);
	void removeInstance(const QString &name);

	void setCreateEnabled(bool createEnabled);

private slots:
	void showSetSync(const QString &name);

signals:
	void closeInstance(const QString &name);
	void performAction(const QString &name, ToolManager::Actions action);
	void setSyncInterval(const QString &name, int interval);

	void createEnabledChanged(bool createEnabled);

private:
	QPointer<QSystemTrayIcon> trayIco;
	QScopedPointer<QMenu, QScopedPointerDeleteLater> menu;
	bool createEnabled;

	QAction *postServers;
	QAction *postClients;

	QHash<QString, QMenu*> elements;

	void reloadTooltip();
};

#endif // MENUMANAGER_H
