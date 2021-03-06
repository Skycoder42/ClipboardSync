#include "app.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QRegularExpression>

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
						 {"a", "auth", "authentication"},
						 "The <password> to secure the server. Only clients knowing the key may connect. "
						 "If not set, no password is required. NOTE: Only use with secure mode, because "
						 "otherwise the key will be send plain.",
						 "password"
					 });
	parser.addOption({
						 {"s", "secure"},
						 "Start the server in secure websocket mode (wss) instead of normal mode (ws). "
						 "The <p12-file> must be a valid PKCS#12 file for the server to load certificate and key. "
						 "If the file is protected by a password, use the -k option",
						 "p12-file"
					 });
	parser.addOption({
						 {"k", "keypass", "keypassphrase"},
						 "The <passphrase> to use to load the PKCS#12 file. if -s is set, you must use this option "
						 "as well, if the file is protected by a password.",
						 "passphrase"
					 });
	parser.addOption({
						 {"l", "local", "localonly"},
						 "Only accept connections from localhost, instead of all connections."
					 });
	parser.process(*this);
	if(parser.positionalArguments().size() != 1) {
		qInfo(qUtf8Printable(parser.helpText()));
		return EXIT_FAILURE;
	}

	this->console = new Console(this);
	this->console->installAsMessageHandler();
	connect(this->console, &Console::commandReceived,
			this, &App::commandReceived,
			Qt::QueuedConnection);

	this->syncServer = new SyncServer(parser.positionalArguments()[0], this);
	connect(this, &App::aboutToQuit,
			this->syncServer, &SyncServer::quitServer);

	if(parser.isSet("s")) {
		if(!this->syncServer->setupSecurity(parser.value("s"), parser.value("k")))
			return EXIT_FAILURE;
	}

	if(this->syncServer->createServer(parser.value("p").toInt(),
									  parser.value("a"),
									  parser.isSet("l")))
		return QCoreApplication::exec();
	else
		return EXIT_FAILURE;
}

void App::commandReceived(const QByteArray &command)
{
	if(command == "sync")
		this->syncServer->syncAll();
	else if(command == "clear")
		this->syncServer->clear();
	else if(command == "port")
		this->syncServer->printPort();
	else if(command == "netinfo")
		this->syncServer->printNetInfo();
	else if(command == "remoteinfo")
		this->syncServer->printRemoteInfo();
	else if(command == "peers")
		this->syncServer->printPeers();
	else if(command == "showconnectinfo")
		this->syncServer->setShowInfo(true);
	else if(command == "hideconnectinfo")
		this->syncServer->setShowInfo(false);
	else if(command.startsWith("close")) {
		const static QRegularExpression regex(QStringLiteral(R"__(^close(?:\s+)([^\s].*)$)__"));
		auto match = regex.match(command);
		if(match.hasMatch())
			this->syncServer->closeNamedClient(match.captured(1));
	} else
		qWarning() << "Unknown command received:" << command;
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
