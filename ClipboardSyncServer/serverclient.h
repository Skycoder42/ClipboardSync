#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QWebSocket>

class ServerClient : public QObject
{
	Q_OBJECT

public:
	explicit ServerClient(QWebSocket *socket, QObject *parent = nullptr);

private slots:
	void error(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocket *socket;
};

#endif // SERVERCLIENT_H
