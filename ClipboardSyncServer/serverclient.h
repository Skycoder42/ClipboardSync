#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QWebSocket>
class SyncServer;

class ServerClient : public QObject
{
	Q_OBJECT

public:
	explicit ServerClient(QWebSocket *socket, SyncServer *parent = nullptr);

	bool validate(const QString &password);

public slots:
	void closeConnection();
	void sendData(const QByteArray &data);

private slots:
	void error(QAbstractSocket::SocketError error);
	void disconnected();
	void sslErrors(const QList<QSslError> &errors);

	void clientMessage(const QByteArray &message);

private:
	SyncServer *server;
	QWebSocket *socket;
};

#endif // SERVERCLIENT_H
