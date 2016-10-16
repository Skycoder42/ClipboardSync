#ifndef SYNCSERVER_H
#define SYNCSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include "serverclient.h"

class SyncServer : public QObject
{
	Q_OBJECT
public:
	explicit SyncServer(const QString &serverName, QObject *parent = nullptr);

	bool setupSecurity(const QString &p12_file, const QString &passphrase);
	bool createServer(int port, const QString &password, bool local);

	quint16 port() const;

public slots:
	void quitServer();

	void syncAll();
	void clear();
	void printPort() const;
	void printNetInfo() const;
	void performSync(ServerClient *origin, const QByteArray &data);

	void closeNamedClient(const QString &name);

private slots:
	void newConnection();

	void acceptError(QAbstractSocket::SocketError socketError);
	void serverError(QWebSocketProtocol::CloseCode closeCode);
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocketServer *server;
	QString serverName;
	QString password;

	QList<ServerClient*> clients;
	QByteArray currentState;
};

#endif // SYNCSERVER_H
