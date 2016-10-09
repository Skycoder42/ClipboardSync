#include "syncserver.h"
#include <QWebSocket>
#include <QWebSocketCorsAuthenticator>
#include <QSslError>
#include <QCoreApplication>

#include <QDebug>

SyncServer::SyncServer(QObject *parent) :
	QObject(parent),
	server(nullptr),
	password(),
	clients()
{}

bool SyncServer::createServer(const QString &serverName, int port, bool secure, const QString &password, bool local)
{
	this->server = new QWebSocketServer(serverName,
										secure ?
											QWebSocketServer::SecureMode :
											QWebSocketServer::NonSecureMode,
										this);
	this->password = password;
	connect(this->server, &QWebSocketServer::newConnection,
			this, &SyncServer::newConnection);
	connect(this->server, &QWebSocketServer::serverError,
			this, &SyncServer::serverError);
//	connect(this->server, &QWebSocketServer::originAuthenticationRequired,
//			this, &SyncServer::originAuthenticationRequired);
//	connect(this->server, &QWebSocketServer::peerVerifyError,
//			this, &SyncServer::peerVerifyError);
	connect(this->server, &QWebSocketServer::sslErrors,
			this, &SyncServer::sslErrors);

	if(port < 0 || port > USHRT_MAX)
		port = 0;
	auto ok = this->server->listen(local ? QHostAddress::LocalHost : QHostAddress::Any, (quint16)port);
	if(ok)
		qDebug() << "Server Running on port" << this->server->serverPort();
	return ok;
}

quint16 SyncServer::port() const
{
	return this->server->serverPort();
}

void SyncServer::quitServer()
{
	this->server->close();
	foreach(auto client, this->clients)
		client->closeConnection();
}

void SyncServer::newConnection()
{
	auto socket = this->server->nextPendingConnection();
	qDebug() << "New client connecting:" << socket->origin();
	auto client = new ServerClient(socket, this);
	if(this->password.isNull() || client->validate(this->password)) {
		this->clients.append(client);
		connect(client, &ServerClient::destroyed, this, [=](){
			this->clients.removeOne(client);
		});

		connect(socket, &QWebSocket::pong, this, [](quint64 dt, QByteArray load){
			qDebug() << dt << load;
		});
		socket->ping("Baum42");
	} else
		client->deleteLater();
}

void SyncServer::acceptError(QAbstractSocket::SocketError socketError)
{
	qWarning() << "Failed to accept new connection ("
			   << socketError
			   << "):"
			   << this->server->errorString().toUtf8();
}

void SyncServer::serverError(QWebSocketProtocol::CloseCode closeCode)
{
	qWarning() << "Failed to connect to peer ("
			   << closeCode
			   << "):"
			   << this->server->errorString().toUtf8();
}

void SyncServer::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << "SSL-Error occured ("
				   << error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}
