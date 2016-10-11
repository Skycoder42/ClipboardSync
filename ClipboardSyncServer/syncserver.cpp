#include "syncserver.h"
#include <QWebSocket>
#include <QWebSocketCorsAuthenticator>
#include <QSslError>
#include <QCoreApplication>
#include <QFile>
#include <QSslKey>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

SyncServer::SyncServer(const QString &serverName, QObject *parent) :
	QObject(parent),
	server(nullptr),
	serverName(serverName),
	password(),
	clients(),
	currentState(QJsonDocument(QJsonObject()).toBinaryData())
{}

bool SyncServer::setupSecurity(const QString &p12_file, const QString &passphrase)
{
	this->server = new QWebSocketServer(this->serverName, QWebSocketServer::SecureMode, this);
	auto config = this->server->sslConfiguration();
	config.setProtocol(QSsl::SecureProtocols);//TODO more security

	QFile file(p12_file);
	if(!file.open(QIODevice::ReadOnly)) {
		qCritical() << "Unable to open PKCS#12 file:"
					<< p12_file;
		return false;
	}
	QSslKey key;
	QSslCertificate cert;
	QList<QSslCertificate> caCertificates;
	auto ok = QSslCertificate::importPkcs12(&file,
											&key,
											&cert,
											&caCertificates,
											passphrase.toUtf8());
	if(!ok) {
		file.close();
		qCritical() << "Unable to load contents from PKCS#12 file"
					<< p12_file;
		return false;
	} else {
		file.close();
		qDebug() << "Successfully PKCS#12 certificate and key!";
		config.setLocalCertificate(cert);
		config.setPrivateKey(key);
		auto caCerts = config.caCertificates();
		caCerts.append(caCertificates);
		config.setCaCertificates(caCerts);

		if(!cert.issuerInfo(QSslCertificate::CommonName).contains(this->server->serverName())) {
			qWarning() << "Certificate does not include the servers name!"
					   << "Known CNs:"
					   << cert.issuerInfo(QSslCertificate::CommonName)
					   << "Server Name:"
					   << this->server->serverName();
		}
	}

	this->server->setSslConfiguration(config);
	return true;
}

bool SyncServer::createServer(int port, const QString &password, bool local)
{
	if(!this->server)
		this->server = new QWebSocketServer(this->serverName, QWebSocketServer::NonSecureMode, this);

	this->password = password;
	connect(this->server, &QWebSocketServer::newConnection,
			this, &SyncServer::newConnection);
	connect(this->server, &QWebSocketServer::acceptError,
			this, &SyncServer::acceptError);
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
	if(ok) {
		qDebug() << "Server Running on port"
				 << this->server->serverPort()
				 << "in"
				 << (this->server->secureMode() == QWebSocketServer::SecureMode ? "secure mode (wss)" : "non secure mode (ws)")
				 << (local ? "(local only)" : "");
	}
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

void SyncServer::syncAll()
{
	foreach(auto client, this->clients)
		client->sendData(this->currentState);
}

void SyncServer::performSync(ServerClient *origin, const QByteArray &data)
{
	this->currentState = data;
	foreach(auto client, this->clients) {
		if(client != origin)
			client->sendData(data);
	}
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

		client->sendData(this->currentState);
	} else
		client->deleteLater();
}

void SyncServer::acceptError(QAbstractSocket::SocketError socketError)
{
	qWarning() << "Failed to accept new connection ("
			   << (int)socketError
			   << "):"
			   << this->server->errorString().toUtf8();
}

void SyncServer::serverError(QWebSocketProtocol::CloseCode closeCode)
{
	qWarning() << "Failed to connect to peer ("
			   << (int)closeCode
			   << "):"
			   << this->server->errorString().toUtf8();
}

void SyncServer::sslErrors(const QList<QSslError> &errors)
{
	foreach(auto error, errors) {
		qWarning() << "SSL-Error occured ("
				   << (int)error.error()
				   << "):"
				   << error.errorString().toUtf8();
	}
}
