#include "syncclient.h"
#include <QUrl>
#include <QNetworkRequest>
#include "synccore.h"

SyncClient::SyncClient(QObject *parent) :
	QObject(parent),
	socket(nullptr)
{}

bool SyncClient::connectSocket(const QString &host, const QString &clientName, bool secure, const QString &password)
{
	this->socket = new QWebSocket(clientName, QWebSocketProtocol::VersionLatest, this);
	connect(this->socket, &QWebSocket::connected,
			this, &SyncClient::connected);
	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &SyncClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &SyncClient::sslErrors);
	connect(this->socket, &QWebSocket::disconnected,
			this, &SyncClient::disconnected);

	QUrl url;
	url.setScheme(secure ? QStringLiteral("wss") : QStringLiteral("ws"));
	url.setAuthority(host);
	if(!url.isValid()) {
		qCritical() << "Invalid host data:"
					<< host
					<< "(Error:"
					<< url.errorString().toUtf8()
					<< ")";
		return false;
	} else {
		if(!password.isNull()) {
			QNetworkRequest request(url);
			request.setRawHeader(SyncCore::AccessKeyHeader, password.toUtf8().toBase64());
			this->socket->open(request);
		} else
			this->socket->open(url);
		return true;
	}
}

void SyncClient::closeConnection()
{
	if(this->socket->state() == QAbstractSocket::ConnectedState) {
		disconnect(this->socket, &QWebSocket::disconnected,
				   this, &SyncClient::disconnected);
		this->socket->close();
	}
}

void SyncClient::connected()
{
	qDebug() << "Connected to server!";
}

void SyncClient::error(QAbstractSocket::SocketError error)
{
	if(error != QAbstractSocket::RemoteHostClosedError) {
		this->socket->close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,
							this->socket->errorString());
		qCritical() << "Socket error occured ("
					<< error
					<< "):"
					<< this->socket->errorString().toUtf8();
	}
}

void SyncClient::disconnected()
{
	qCritical() << "Server closed the connection ("
				<< this->socket->closeCode()
				<< "):"
				<< this->socket->closeReason();
}

void SyncClient::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << "SSL-Error occured ("
				   << error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}
