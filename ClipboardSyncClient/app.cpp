#include "app.h"
#include <QCommandLineParser>

#include <QDebug>

App::App(int &argc, char **argv) :
	QGuiApplication(argc, argv),
	console(nullptr),
	client(nullptr),
	clipController(nullptr)
{
	QCoreApplication::setApplicationName(TARGET);
	QCoreApplication::setApplicationVersion(VERSION);
	QCoreApplication::setOrganizationName(COMPANY);
	QCoreApplication::setOrganizationDomain("com.Skycoder42");
}

int App::exec()//TODO single instance PER SERVER
{
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("name",
								 "The name of this client. Will be used to identify clients on the server.");
	parser.addPositionalArgument("address",
								 "The host address of the target server. Must include the port!");
	parser.addOption({
						 {"s", "secure"},
						 "Connect to the server in secure websocket mode (wss) instead of normal mode (ws). "
						 "You can specify a <cert_file> that the client will use for the server validation. "
						 "To accept any certificate, simply pass \"any\" as parameter to this option. If you simply want "
						 "to use the default ones, use \"default\" instead.",
						 "cert_file"
					 });
	parser.addOption({
						 {"f", "format"},
						 "The <format> of the certificate file. Supported are pem and der. The default is pem. "
						 "You only need to set this option in combination with the -s option and a certificate with "
						 "a format other them pem.",
						 "format",
						 "pem"
					 });
	parser.addOption({
						 {"a", "auth", "authentication"},
						 "The <password>, if required. A server may or may not require a password.",
						 "password"
					 });
	parser.process(*this);
	if (parser.positionalArguments().size() != 2) {
		qInfo(qUtf8Printable(parser.helpText()));
		return EXIT_FAILURE;
	}

	//console
	this->console = new Console(this);
	this->console->installAsMessageHandler();
	connect(this->console, &Console::commandReceived,
			this, &App::commandReceived,
			Qt::QueuedConnection);

	//client
	this->client = new SyncClient(parser.positionalArguments()[0], this);
	connect(this, &App::aboutToQuit,
			this->client, &SyncClient::closeConnection);

	if(parser.isSet("s")) {
		if(!this->client->setupSecurity(parser.value("s"), parser.value("f")))
			return EXIT_FAILURE;
	}
	if(!this->client->connectSocket(parser.positionalArguments()[1], parser.value("a")))
		return EXIT_FAILURE;

	//clipController
	this->clipController = new ClipboardController(this);
	connect(this->clipController, &ClipboardController::clipboardChanged,
			this->client, &SyncClient::sendData);
	connect(this->client, &SyncClient::dataReceived,
			this->clipController, &ClipboardController::setClipboard);

	return QCoreApplication::exec();
}

void App::commandReceived(const QByteArray &command)
{
	if(command == "sync")
		this->clipController->syncNow();
	else if(command == "clear")
		this->clipController->clear();
	else if(command == "showconnectinfo")
		this->client->setShowInfo(true);
	else if(command == "hideconnectinfo")
		this->client->setShowInfo(false);
	else
		qWarning() << "Unknown command received:" << command;
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
