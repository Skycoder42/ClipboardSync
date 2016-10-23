#include "app.h"
#include <QSsl>
#include <QIcon>
#include <dialogmaster.h>
#include "mainwizard.h"
#ifdef Q_OS_OSX
#include "dockhidehelper.h"
#endif

Q_DECLARE_METATYPE(QSsl::EncodingFormat)

App::App(int &argc, char **argv) :
	QApplication(argc, argv),
	trayIco(nullptr),
	toolManager(nullptr),
	menuManager(nullptr),
	logDialogs(),
	peersDialogs()
{
	setApplicationName(TARGET);
	setApplicationVersion(VERSION);
	setOrganizationName(COMPANY);
	setOrganizationDomain(QStringLiteral("com.Skycoder42"));
	setApplicationDisplayName(DISPLAY_NAME);
	setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));
	setQuitOnLastWindowClosed(false);

	qRegisterMetaType<QSsl::EncodingFormat>();

	this->init();
}

int App::exec()
{
	this->trayIco->show();
	if(!this->toolManager->initStartup())
		this->showCreate();

	return QApplication::exec();
}

bool App::testNameUnique(const QString &name) const
{
	return !this->toolManager->hasName(name);
}

void App::showCreate()
{
	this->menuManager->setCreateEnabled(false);
	auto wiz = MainWizard::createInstance(this->toolManager);
	connect(wiz, &MainWizard::destroyed, this, [this](){
		this->menuManager->setCreateEnabled(true);
	});
}

void App::showMessage(QtMsgType type, const QString &title, const QString &message)
{
	QSystemTrayIcon::MessageIcon icon;
	switch(type) {
	case QtMsgType::QtInfoMsg:
		icon = QSystemTrayIcon::Information;
		break;
	case QtMsgType::QtWarningMsg:
		icon = QSystemTrayIcon::Warning;
		break;
	case QtMsgType::QtCriticalMsg:
	case QtMsgType::QtFatalMsg:
		icon = QSystemTrayIcon::Critical;
		break;
	default:
		icon = QSystemTrayIcon::NoIcon;
		break;
	}

	this->trayIco->showMessage(title, message, icon);
}

void App::serverStatusLoaded(const QString &name, ToolManager::ServerInfo info)
{
	auto config = DialogMaster::createInformation();
	config.windowTitle = tr("Server created");
	config.title = tr("Server %1 — Running").arg(name);
	config.text = tr("Server \"%1\" is running. It runs on port %2. "
					 "Check the Details for a list of all IPs you can access it with.")
				  .arg(name)
				  .arg(info.port);
	config.details = tr("Port: %1\n").arg(info.port);
	if(info.remoteAddress != QStringLiteral("0.0.0.0"))
		config.details += tr("Remote Address (Internet): %1\n").arg(info.remoteAddress);
	config.details += tr("Known local IP-Addresses:");
	foreach(auto info, info.localAddresses)
		config.details.append(tr("\n - ") + info);

	auto box = DialogMaster::createMessageBox(config);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
	box->raise();
	box->activateWindow();
	this->alert(box);
}

void App::showLog(const QString &name, const QByteArray &log)
{
	auto logDiag = this->logDialogs.value(name, nullptr);
	if(!logDiag) {
		logDiag = new LogDialog(this->toolManager->createTitle(name, tr("Debug Log")));
		connect(logDiag, &LogDialog::reloadTriggered, this, [=](){
			this->toolManager->performAction(name, ToolManager::Log);
		});
		connect(logDiag, &LogDialog::destroyed, this, [=](){
			this->logDialogs.remove(name);
		}, Qt::DirectConnection);
		this->logDialogs.insert(name, logDiag);
	}

	logDiag->reloadLog(log);
	logDiag->popup();
}

void App::showPeers(const QString &name, QStandardItemModel *peerModel)
{
	auto peerDiag = this->peersDialogs.value(name, nullptr);
	if(!peerDiag) {
		peerDiag = new PeersDialog(this->toolManager->createTitle(name, tr("Connected Peers")));
		connect(peerDiag, &PeersDialog::reloadTriggered, this, [=](){
			this->toolManager->performAction(name, ToolManager::Peers);
		});
		connect(peerDiag, &PeersDialog::closeClient, this, [=](QString clientName){
			this->toolManager->removeClient(name, clientName);
		});
		connect(peerDiag, &PeersDialog::destroyed, this, [=](){
			this->peersDialogs.remove(name);
		}, Qt::DirectConnection);
		this->peersDialogs.insert(name, peerDiag);
	}

	peerDiag->reloadPeers(peerModel);
	peerDiag->popup();
}

void App::init()
{
#ifdef Q_OS_OSX
    new DockHideHelper(this);
#endif

	this->trayIco = new QSystemTrayIcon(windowIcon(), this);

	this->toolManager = new ToolManager(this);
	connect(this->toolManager, &ToolManager::showMessage,
			this, &App::showMessage);

	this->menuManager = new MenuManager(this->trayIco, this);
	connect(this->toolManager, &ToolManager::allowCreate,
			this->menuManager, &MenuManager::setCreateEnabled);
	connect(this->toolManager, &ToolManager::importDone,
			this->menuManager, &MenuManager::completeImport);
	connect(this->toolManager, &ToolManager::serverCreated,
			this->menuManager, &MenuManager::addServer);
	connect(this->toolManager, &ToolManager::clientCreated,
			this->menuManager, &MenuManager::addClient);
	connect(this->toolManager, &ToolManager::instanceClosed,
			this->menuManager, &MenuManager::removeInstance);

	connect(this->menuManager, &MenuManager::doImport,
			this->toolManager, &ToolManager::importInstance);
	connect(this->menuManager, &MenuManager::performAction,
			this->toolManager, &ToolManager::performAction);
	connect(this->menuManager, &MenuManager::setSyncInterval,
			this->toolManager, &ToolManager::setSyncInterval);

	connect(this->toolManager, &ToolManager::serverStatusLoaded,
			this, &App::serverStatusLoaded);
	connect(this->toolManager, &ToolManager::showLog,
			this, &App::showLog);
	connect(this->toolManager, &ToolManager::showPeers,
			this, &App::showPeers);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
