#include "syncclient.h"
#include <QUrl>
#include <QNetworkRequest>
#include "synccore.h"

SyncClient::SyncClient(const QString &clientName, QObject *parent) :
	QObject(parent),
	socket(new QWebSocket(clientName, QWebSocketProtocol::VersionLatest, this)),
	isSecure(false)
{
	connect(this->socket, &QWebSocket::connected,
			this, &SyncClient::connected);
	connect(this->socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
			this, &SyncClient::error);
	connect(this->socket, &QWebSocket::sslErrors,
			this, &SyncClient::sslErrors);
	connect(this->socket, &QWebSocket::disconnected,
			this, &SyncClient::disconnected);

	qDebug() << "Created client with name" << clientName;
}

bool SyncClient::setupSecurity(const QString &cert_file, const QString &format)
{
	auto config = this->socket->sslConfiguration();
	config.setProtocol(QSsl::SecureProtocols);//TODO more security

	if(cert_file == QStringLiteral("any")) {
		qWarning() << "Client will accept all certificates without validation!";
		config.setPeerVerifyMode(QSslSocket::VerifyNone);
	} else if(cert_file != QStringLiteral("default")) {
		QSsl::EncodingFormat eFormat;
		if(format == QStringLiteral("der"))
			eFormat = QSsl::Der;
		else
			eFormat = QSsl::Pem;

		auto certs = QSslCertificate::fromPath(cert_file, eFormat);
		if(certs.isEmpty()) {
			qCritical() << "Unable to load certificate from file:"
						<< cert_file
						<< "with format"
						<< format;
			return false;
		} else {
			qDebug() << "Successfully loaded certificate(s)!";
			auto caCerts = config.caCertificates();
			caCerts.append(certs);
			config.setCaCertificates(caCerts);
		}
	}
	this->socket->setSslConfiguration(config);

	this->isSecure = true;
	return true;
}

bool SyncClient::connectSocket(const QString &host, const QString &password)
{
	QUrl url;
	url.setScheme(this->isSecure ? QStringLiteral("wss") : QStringLiteral("ws"));
	url.setAuthority(host);
	if(!url.isValid()) {
		qCritical() << "Invalid host data:"
					<< host
					<< "(Error:"
					<< url.errorString().toUtf8()
					<< ")";
		return false;
	} else {
		qDebug() << "Connecting to server at" << url.toString();
		if(!password.isEmpty()) {
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
