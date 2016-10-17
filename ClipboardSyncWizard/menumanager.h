#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QScopedPointer>
#include <QAction>

class MenuManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool createEnabled READ isCreateEnabled WRITE setCreateEnabled NOTIFY createEnabledChanged)

public:
	explicit MenuManager(QSystemTrayIcon *trayIco, QObject *parent = nullptr);

	bool isCreateEnabled() const;

public slots:
	void addServer(const QString &name);

	void setCreateEnabled(bool createEnabled);

signals:
	void createEnabledChanged(bool createEnabled);

private:
	QSystemTrayIcon *trayIco;
	QScopedPointer<QMenu, QScopedPointerDeleteLater> menu;
	bool createEnabled;

	QAction *postServers;
	QAction *postClients;

	QHash<QString, QMenu*> elements;

	void reloadTooltip();
};

#endif // MENUMANAGER_H
