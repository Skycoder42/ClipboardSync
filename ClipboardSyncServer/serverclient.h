#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QWebSocket>

class ServerClient : public QObject
{
	Q_OBJECT

public:
	explicit ServerClient(QWebSocket *socket, QObject *parent = nullptr);

	bool validate(const QString &password);

public slots:
	void closeConnection();

private slots:
	void error(QAbstractSocket::SocketError error);
	void disconnected();
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocket *socket;
};

#endif // SERVERCLIENT_H
