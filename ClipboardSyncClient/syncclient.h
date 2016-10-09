#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <QObject>
#include <QWebSocket>

class SyncClient : public QObject
{
	Q_OBJECT
public:
	explicit SyncClient(QObject *parent = nullptr);

	void connectSocket(const QString &host, const QString &serverName, bool secure);

private:
	QWebSocket *socket;
};

#endif // SYNCCLIENT_H
