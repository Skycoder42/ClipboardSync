#ifndef SYNCSERVER_H
#define SYNCSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include "serverclient.h"

class SyncServer : public QObject
{
	Q_OBJECT
public:
	explicit SyncServer(QObject *parent = nullptr);

	bool createServer(const QString &serverName,
					  int port,
					  bool secure, const QString &password,
					  bool local);

	quint16 port() const;

public slots:
	void quitServer();

private slots:
	void newConnection();

	void acceptError(QAbstractSocket::SocketError socketError);
	void serverError(QWebSocketProtocol::CloseCode closeCode);
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocketServer *server;
	QString password;
	QList<ServerClient*> clients;
};

#endif // SYNCSERVER_H
