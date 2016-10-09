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
	parser.addPositionalArgument("address",
								 "The host address of the target server. Must include the port!");
	parser.addPositionalArgument("name",
								 "The name of this client. Will be used to identify clients on the server.");
	parser.addPositionalArgument("password",
								 "The password, if required. A server may or may not require a password.",
								 "[password]");
	parser.addOption({
						 {"s", "secure"},
						 "Connect to the server in secure websocket mode (wss) instead of normal mode (ws)."
					 });
	parser.process(*this);

	QString pass;
	switch (parser.positionalArguments().size()) {
	case 3:
		pass = parser.positionalArguments()[2];
	case 2:
		if(this->init(parser.positionalArguments()[0],
					  parser.positionalArguments()[1],
					  parser.isSet("s"),
					  pass))
			return QCoreApplication::exec();
		else
			return EXIT_FAILURE;
		break;
	default:
		qDebug() << parser.helpText();
		return EXIT_FAILURE;
	}
}

bool App::init(const QString &address, const QString &clientName, bool secure, const QString &password)
{
	this->console = new Console(this);
	this->console->installAsMessageHandler();

	this->client = new SyncClient(this);
	connect(this, &App::aboutToQuit,
			this->client, &SyncClient::closeConnection);
	return this->client->connectSocket(address, clientName, secure, password);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
