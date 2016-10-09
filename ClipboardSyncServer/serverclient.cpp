#include "serverclient.h"
#include "synccore.h"

ServerClient::ServerClient(QWebSocket *socket, QObject *parent) :
	QObject(parent),
	socket(socket)
{
	this->socket->setParent(this);

	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &ServerClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &ServerClient::sslErrors);
	connect(this->socket, &QWebSocket::disconnected,
			this, &ServerClient::disconnected);
}

bool ServerClient::validate(const QString &password)
{
	auto request = this->socket->request();
	auto authData = request.rawHeader(SyncCore::AccessKeyHeader);
	auto remotePw = QString::fromUtf8(QByteArray::fromBase64(authData));
	if(password == remotePw)
		return true;
	else {
		qWarning() << "Client authentification failed for client:"
				   << this->socket->origin();
		disconnect(this->socket, &QWebSocket::disconnected,
				   this, &ServerClient::disconnected);
		this->socket->close(QWebSocketProtocol::CloseCodePolicyViolated,
							"Invalid Password");
		return false;
	}
}

void ServerClient::closeConnection()
{
	if(this->socket->state() == QAbstractSocket::ConnectedState) {
		disconnect(this->socket, &QWebSocket::disconnected,
				   this, &ServerClient::disconnected);
		this->socket->close();
	}
}

void ServerClient::error(QAbstractSocket::SocketError error)
{
	if(error != QAbstractSocket::RemoteHostClosedError) {
		qWarning() << "Socket error occured ("
				   << error
				   << "):"
				   << this->socket->errorString().toUtf8();

		this->socket->close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,
							this->socket->errorString());
	}

	this->deleteLater();
}

void ServerClient::disconnected()
{
	qWarning() << "Client closed the connection ("
			   << this->socket->closeCode()
			   << "):"
			   << this->socket->closeReason();
}

void ServerClient::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << "SSL-Error occured ("
				   << error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}

