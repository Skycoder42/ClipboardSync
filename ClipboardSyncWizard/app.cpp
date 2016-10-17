#include "app.h"
#include <QSsl>
#include <QIcon>
#include <dialogmaster.h>
#include "mainwizard.h"

Q_DECLARE_METATYPE(QSsl::EncodingFormat)

App::App(int &argc, char **argv) :
	QApplication(argc, argv),
	trayIco(nullptr),
	toolManager(nullptr),
	menuManager(nullptr)
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

	//DEBUG --> dynamic only if no config!
	this->menuManager->setCreateEnabled(false);
	if(!MainWizard::createInstance(this->toolManager))
		return EXIT_FAILURE;
	this->menuManager->setCreateEnabled(true);

	return QApplication::exec();
}

bool App::testNameUnique(const QString &name) const
{
	return !this->toolManager->hasName(name);
}

void App::showCreate()
{
	this->menuManager->setCreateEnabled(false);
	MainWizard::createInstance(this->toolManager);
	this->menuManager->setCreateEnabled(true);
}

void App::errorOccured(bool isServer, const QString &name, const QString &error)
{
	DialogMaster::critical(nullptr,
						   error,
						   (isServer ?
							   tr("Server %1 — Error occured!") :
							   tr("Client %1 — Error occured!"))
						   .arg(name));
}

void App::init()
{
	this->trayIco = new QSystemTrayIcon(windowIcon(), this);

	this->toolManager = new ToolManager(this);
	connect(this->toolManager, &ToolManager::errorOccured,
			this, &App::errorOccured);

	this->menuManager = new MenuManager(this->trayIco, this);
	connect(this->toolManager, &ToolManager::serverCreated,
			this->menuManager, &MenuManager::addServer);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
