#include "serverclient.h"

ServerClient::ServerClient(QWebSocket *socket, QObject *parent) :
	QObject(parent),
	socket(socket)
{
	this->socket->setParent(this);

	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &ServerClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &ServerClient::sslErrors);
}

void ServerClient::error(QAbstractSocket::SocketError error)
{
	if(error == QAbstractSocket::RemoteHostClosedError) {
		qWarning() << "Client closed the connection ("
				   << this->socket->closeCode()
				   << "):"
				   << this->socket->closeReason();
	} else {
		qWarning() << "Socket error occured ("
				   << error
				   << "):"
				   << this->socket->errorString().toUtf8();

		this->socket->close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,
							this->socket->errorString());
	}

	this->deleteLater();
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

