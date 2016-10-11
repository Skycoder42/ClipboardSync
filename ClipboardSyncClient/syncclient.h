#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <QObject>
#include <QWebSocket>

class SyncClient : public QObject
{
	Q_OBJECT
public:
	explicit SyncClient(const QString &clientName, QObject *parent = nullptr);

	bool setupSecurity(const QString &cert_file, const QString &format);
	bool connectSocket(const QString &host, const QString &password = QString());

public slots:
	void closeConnection();

	void sendData(const QByteArray &data);

signals:
	void dataReceived(const QByteArray &data);

private slots:
	void connected();
	void error(QAbstractSocket::SocketError error);
	void disconnected();
	void sslErrors(const QList<QSslError> &errors);

private:
	QWebSocket *socket;
	bool isSecure;
};

#endif // SYNCCLIENT_H
