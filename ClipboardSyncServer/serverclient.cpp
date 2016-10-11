#include "serverclient.h"
#include "synccore.h"
#include "syncserver.h"

ServerClient::ServerClient(QWebSocket *socket, SyncServer *parent) :
	QObject(parent),
	server(parent),
	socket(socket)
{
	this->socket->setParent(this);

	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &ServerClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &ServerClient::sslErrors);
	connect(this->socket, &QWebSocket::disconnected,
			this, &ServerClient::disconnected);

	connect(this->socket, &QWebSocket::binaryMessageReceived,
			this, &ServerClient::clientMessage);
}

bool ServerClient::validate(const QString &password)
{
	auto request = this->socket->request();
	auto authData = request.rawHeader(SyncCore::AccessKeyHeader);
	auto remotePw = QString::fromUtf8(QByteArray::fromBase64(authData));
	if(password == remotePw)
		return true;
	else {
		qWarning() << qPrintable(this->socket->origin()) << "-->"
				   << "Client authentification failed! Invalid Password.";
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

void ServerClient::sendData(const QByteArray &data)
{
	this->socket->sendBinaryMessage(data);
}

void ServerClient::error(QAbstractSocket::SocketError error)
{
	if(error != QAbstractSocket::RemoteHostClosedError) {
		qWarning() << qPrintable(this->socket->origin()) << "-->"
				   << "Socket error occured ("
				   << (int)error
				   << "):"
				   << this->socket->errorString().toUtf8();

		this->socket->close(QWebSocketProtocol::CloseCodeAbnormalDisconnection,
							this->socket->errorString());
	}

	this->deleteLater();
}

void ServerClient::disconnected()
{
	qWarning() << qPrintable(this->socket->origin()) << "-->"
			   << "Client closed the connection ("
			   << (int)this->socket->closeCode()
			   << "):"
			   << this->socket->closeReason();
}

void ServerClient::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << qPrintable(this->socket->origin()) << "-->"
				   << "SSL-Error occured ("
				   << (int)error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}

void ServerClient::clientMessage(const QByteArray &message)
{
	qDebug() << qPrintable(this->socket->origin()) << "-->"
			 << "Sent new clipboard state";
	this->server->performSync(this, message);
}

