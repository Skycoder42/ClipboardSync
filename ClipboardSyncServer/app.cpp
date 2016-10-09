#include "app.h"
#include <QCommandLineParser>
#include <QDebug>

App::App(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	reader(nullptr),
	syncServer(nullptr)
{
	QCoreApplication::setApplicationName(TARGET);
	QCoreApplication::setApplicationVersion(VERSION);
	QCoreApplication::setOrganizationName(COMPANY);
	QCoreApplication::setOrganizationDomain("com.Skycoder42");
}

int App::exec()
{
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("name", "The name of the server. Must be known by the clients");
	parser.addPositionalArgument("port", "The server port. Choosen automatically, if not specified.", "[port]");
	parser.addOption({
						 {"s", "secure"},
						 "Start the server in secure websocket mode (wss) instead of normal mode (ws)."
					 });

	parser.addOption({
						 {"l", "local"},
						 "Only accept connections from localhost, instead of all connections."
					 });
	parser.process(*this);

	auto port = 0;
	QString name;
	switch (parser.positionalArguments().size()) {
	default:
	case 2:
		port = parser.positionalArguments()[1].toInt();
	case 1:
		name = parser.positionalArguments()[0];
		break;
	case 0:
		qDebug() << parser.helpText();
		return EXIT_FAILURE;
	}

	if(this->init(name, port, parser.isSet("s"), parser.isSet("l")))
		return QCoreApplication::exec();
	else
		return EXIT_FAILURE;
}

bool App::init(const QString &serverName, int port, bool secure, bool local)
{
	this->reader = new InputReader(this);

	this->syncServer = new SyncServer(this);
	connect(this, &App::aboutToQuit,
			this->syncServer, &SyncServer::quitServer);
	connect(this->syncServer, &SyncServer::errorOccured, this, [](QString error, int code){
		qDebug() << "Error:" << error << code;
	});

	auto ok = this->syncServer->createServer(serverName, port, secure, local);
	if(ok)
		qDebug() << "Server Running on port" << this->syncServer->port();
	else
		qWarning() << this->syncServer->lastError();
	return ok;
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
