#include "app.h"
#include <QCommandLineParser>

#include <QDebug>

App::App(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	console(nullptr),
	client(nullptr)
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
	parser.addPositionalArgument("address", "The host address of the target server. Must include the port!");
	parser.addPositionalArgument("name", "The name of the server. Must be known by the clients");
	parser.addOption({
						 {"s", "secure"},
						 "Connect to the server in secure websocket mode (wss) instead of normal mode (ws)."
					 });
	parser.process(*this);

	if(parser.positionalArguments().size() != 2) {
		qDebug() << parser.helpText();
		return EXIT_FAILURE;
	} else {
		if(this->init(parser.positionalArguments()[0],
					  parser.positionalArguments()[1],
					  parser.isSet("s")))
			return QCoreApplication::exec();
		else
			return EXIT_FAILURE;
	}
}

bool App::init(const QString &address, const QString &serverName, bool secure)
{
	this->console = new Console(this);
	this->console->installAsMessageHandler();

	this->client = new SyncClient(this);
	return this->client->connectSocket(address, serverName, secure);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
