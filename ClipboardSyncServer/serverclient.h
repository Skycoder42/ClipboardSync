#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonValue>
class SyncServer;

class ServerClient : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool showInfo READ isInfoShown WRITE setShowInfo)

public:
	explicit ServerClient(QWebSocket *socket, bool showInfo, SyncServer *parent = nullptr);

	bool validate(const QString &password);

	QString name() const;
	void printConnected() const;
	QJsonValue peerInfo() const;

	bool isInfoShown() const;

public slots:
	void closeConnection();
	void sendData(const QByteArray &data);
	void sendName(const QString &name);

	void setShowInfo(bool showInfo);

private slots:
	void error(QAbstractSocket::SocketError error);
	void disconnected();
	void sslErrors(const QList<QSslError> &errors);

	void clientMessage(const QByteArray &message);

private:
	SyncServer *server;
	QWebSocket *socket;
	bool showInfo;
};

#endif // SERVERCLIENT_H
