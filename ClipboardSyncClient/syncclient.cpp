#include "syncclient.h"
#include <QUrl>

SyncClient::SyncClient(QObject *parent) :
	QObject(parent),
	socket(nullptr)
{}

void SyncClient::connectSocket(const QString &host, const QString &serverName, bool secure)
{
	this->socket = new QWebSocket(serverName,QWebSocketProtocol::VersionLatest, this);
	QUrl url;
	url.setScheme(secure ? "wss" : "ws");
	url.setAuthority(host);

	qDebug(url.isValid() ? "ok" : "saodifsodf");
	this->socket->open(url);
}
