#include "syncclient.h"
#include <QUrl>

SyncClient::SyncClient(QObject *parent) :
	QObject(parent),
	socket(nullptr)
{}

bool SyncClient::connectSocket(const QString &host, const QString &serverName, bool secure)
{
	this->socket = new QWebSocket(serverName, QWebSocketProtocol::VersionLatest, this);
	connect(this->socket, &QWebSocket::connected,
			this, &SyncClient::connected);
	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &SyncClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &SyncClient::sslErrors);

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
		this->socket->open(url);
		return true;
	}
}

void SyncClient::connected()
{
	qDebug() << "connected!";
}

void SyncClient::error(QAbstractSocket::SocketError error)
{
	if(error == QAbstractSocket::RemoteHostClosedError) {
		qCritical() << "Server closed the connection ("
					<< this->socket->closeCode()
					<< "):"
					<< this->socket->closeReason();
	} else {
		this->socket->close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,
							this->socket->errorString());
		qCritical() << "Socket error occured ("
					<< error
					<< "):"
					<< this->socket->errorString().toUtf8();
	}
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
