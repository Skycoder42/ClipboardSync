#include "syncserver.h"
#include <QWebSocket>
#include <QWebSocketCorsAuthenticator>
#include <QSslError>

#include <QDebug>

SyncServer::SyncServer(QObject *parent) :
	QObject(parent),
	server(nullptr),
	errorString()
{}

bool SyncServer::createServer(const QString &serverName, int port, bool secure, bool local)
{
	this->server = new QWebSocketServer(serverName,
										secure ?
											QWebSocketServer::SecureMode :
											QWebSocketServer::NonSecureMode,
										this);
	connect(this->server, &QWebSocketServer::newConnection,
			this, &SyncServer::newConnection);
	connect(this->server, &QWebSocketServer::serverError,
			this, &SyncServer::serverError);
	connect(this->server, &QWebSocketServer::originAuthenticationRequired,
			this, &SyncServer::originAuthenticationRequired);
	connect(this->server, &QWebSocketServer::peerVerifyError,
			this, &SyncServer::peerVerifyError);
	connect(this->server, &QWebSocketServer::sslErrors,
			this, &SyncServer::sslErrors);

	if(port < 0 || port > USHRT_MAX)
		port = 0;
	return this->server->listen(local ? QHostAddress::LocalHost : QHostAddress::Any, (quint16)port);
}

quint16 SyncServer::port() const
{
	return this->server->serverPort();
}

QString SyncServer::lastError() const
{
	return this->errorString;
}

void SyncServer::quitServer()
{
	this->server->close();
}

void SyncServer::newConnection()
{
	auto socket = this->server->nextPendingConnection();
	connect(socket, &QWebSocket::pong, this, [](quint64 dt, QByteArray load){
		qDebug() << dt << load;
	});
	socket->ping("Baum42");
}

void SyncServer::acceptError(QAbstractSocket::SocketError socketError)
{
	this->errorString = this->server->errorString();
	emit errorOccured(this->errorString, socketError);
}

void SyncServer::serverError(QWebSocketProtocol::CloseCode closeCode)
{
	this->errorString = this->server->errorString();
	emit errorOccured(this->errorString, closeCode);
}

void SyncServer::originAuthenticationRequired(QWebSocketCorsAuthenticator *pAuthenticator)
{
	qDebug() << pAuthenticator->origin();
	pAuthenticator->setAllowed(true);
}

void SyncServer::peerVerifyError(const QSslError &error)
{
	this->errorString = error.errorString();
	emit errorOccured(this->errorString, error.error());
}

void SyncServer::sslErrors(const QList<QSslError> &errors)
{
	foreach (auto error, errors)
		this->peerVerifyError(error);
}
