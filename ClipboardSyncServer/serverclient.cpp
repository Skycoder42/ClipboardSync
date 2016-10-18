#include "serverclient.h"
#include "synccore.h"
#include "syncserver.h"

ServerClient::ServerClient(QWebSocket *socket, bool showInfo, SyncServer *parent) :
	QObject(parent),
	server(parent),
	socket(socket),
	showInfo(showInfo)
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
		qWarning() << qUtf8Printable(this->socket->origin()) << "-->"
				   << "Client authentification failed! Invalid Password.";
		disconnect(this->socket, &QWebSocket::disconnected,
				   this, &ServerClient::disconnected);
		this->socket->close(QWebSocketProtocol::CloseCodePolicyViolated,
							"Invalid Password");
		return false;
	}
}

QString ServerClient::name() const
{
	return this->socket->origin();
}

void ServerClient::printConnected() const
{
	if(this->showInfo) {
		qInfo() << "info:"
				<< qUtf8Printable(tr("New Client connected!;A new client with the name \"%1\"has connected to the server.")
							  .arg(socket->origin()));
	}
	qDebug() << "New client connected:" << socket->origin();
}

bool ServerClient::isInfoShown() const
{
	return this->showInfo;
}

void ServerClient::closeConnection()
{
	qDebug() << qUtf8Printable(this->socket->origin()) << "-->"
			 << "Client was closed by the server";
	if(this->socket->state() == QAbstractSocket::ConnectedState) {
		disconnect(this->socket, &QWebSocket::disconnected,
				   this, &ServerClient::disconnected);
		this->socket->close();
		this->deleteLater();
	}
}

void ServerClient::sendData(const QByteArray &data)
{
	if(!data.isEmpty())
		this->socket->sendBinaryMessage(data);
}

void ServerClient::setShowInfo(bool showInfo)
{
	this->showInfo = showInfo;
}

void ServerClient::error(QAbstractSocket::SocketError error)
{
	if(error != QAbstractSocket::RemoteHostClosedError) {
		qWarning() << qUtf8Printable(this->socket->origin()) << "-->"
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
	qWarning() << qUtf8Printable(this->socket->origin()) << "-->"
			   << "Client closed the connection ("
			   << (int)this->socket->closeCode()
			   << "):"
			   << this->socket->closeReason();
	this->deleteLater();
}

void ServerClient::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << qUtf8Printable(this->socket->origin()) << "-->"
				   << "SSL-Error occured ("
				   << (int)error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}

void ServerClient::clientMessage(const QByteArray &message)
{
	qDebug() << qUtf8Printable(this->socket->origin()) << "-->"
			 << "Sent new clipboard state";
	this->server->performSync(this, message);
}

