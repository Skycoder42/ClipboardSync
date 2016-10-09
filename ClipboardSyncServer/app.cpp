#include "app.h"
#include <QCommandLineParser>
#include <QDebug>

App::App(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	console(nullptr),
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
	parser.addOption({
						 {"p", "port"},
						 "The server <port>. Choosen automatically, if not specified.",
						 "port",
						 "0"
					 });
	parser.addOption({
						 {"a", "password", "access-key"},
						 "The <password> to secure the server. Only clients knowing the key may connect. "
						 "If not set, no password is required. NOTE: Only use with secure mode, because "
						 "otherwise the key will be send plain.",
						 "password"
					 });
	parser.addOption({
						 {"s", "secure"},
						 "Start the server in secure websocket mode (wss) instead of normal mode (ws)."
					 });
	parser.addOption({
						 {"l", "local"},
						 "Only accept connections from localhost, instead of all connections."
					 });
	parser.process(*this);
	if(parser.positionalArguments().size() != 1) {
		qInfo(qPrintable(parser.helpText()));
		return EXIT_FAILURE;
	}

	if(this->init(parser.positionalArguments()[0],
				  parser.value("p").toInt(),
				  parser.isSet("s"),
				  parser.value("a"),
				  parser.isSet("l")))
		return QCoreApplication::exec();
	else
		return EXIT_FAILURE;
}

bool App::init(const QString &serverName, int port, bool secure, const QString &password, bool local)
{
	this->console = new Console(this);
	this->console->installAsMessageHandler();

	this->syncServer = new SyncServer(this);
	connect(this, &App::aboutToQuit,
			this->syncServer, &SyncServer::quitServer);

	return this->syncServer->createServer(serverName, port, secure, password, local);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
