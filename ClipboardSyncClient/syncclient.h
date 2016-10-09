#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <QObject>
#include <QWebSocket>

class SyncClient : public QObject
{
	Q_OBJECT
public:
	explicit SyncClient(QObject *parent = nullptr);

	bool connectSocket(const QString &host, const QString &serverName, bool secure);

private slots:
	void connected();
	void error(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocket *socket;
};

#endif // SYNCCLIENT_H
