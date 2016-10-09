#ifndef SYNCSERVER_H
#define SYNCSERVER_H

#include <QObject>
#include <QWebSocketServer>

class SyncServer : public QObject
{
	Q_OBJECT
public:
	explicit SyncServer(QObject *parent = nullptr);

	bool createServer(const QString &serverName,
					  int port,
					  bool secure,
					  bool local);

	quint16 port() const;
	QString lastError() const;

public slots:
	void quitServer();

signals:
	void errorOccured(const QString &errorString, int code);

private slots:
	void newConnection();

	void acceptError(QAbstractSocket::SocketError socketError);
	void serverError(QWebSocketProtocol::CloseCode closeCode);
	void originAuthenticationRequired(QWebSocketCorsAuthenticator *pAuthenticator);
	void peerVerifyError(const QSslError &error);
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocketServer *server;
	QString errorString;
};

#endif // SYNCSERVER_H
