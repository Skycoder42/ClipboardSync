#include "app.h"
#include <QSsl>
#include <QIcon>
#include <dialogmaster.h>
#include "mainwizard.h"

Q_DECLARE_METATYPE(QSsl::EncodingFormat)

App::App(int &argc, char **argv) :
	QApplication(argc, argv),
	trayIco(nullptr),
	trayMenu(nullptr),
	manager(nullptr)
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

App::~App()
{
	delete this->trayMenu;
}

int App::exec()
{
	this->trayIco->show();

	//DEBUG --> dynamic only if no config!
	this->trayMenu->actions().first()->setEnabled(false);
	if(!MainWizard::createInstance(this->manager))
		return EXIT_FAILURE;
	this->trayMenu->actions().first()->setEnabled(true);

	return QApplication::exec();
}

void App::showCreate()
{
	this->trayMenu->actions().first()->setEnabled(false);
	MainWizard::createInstance(this->manager);
	this->trayMenu->actions().first()->setEnabled(true);
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
	this->trayMenu = new QMenu();
	this->trayMenu->addAction(tr("Add Instance"), this, &App::showCreate);
	this->trayMenu->addSeparator();
	this->trayMenu->addAction(tr("Quit"), this, &App::quit);

	this->trayIco = new QSystemTrayIcon(windowIcon(), this);
	this->trayIco->setToolTip(tr("%1 — Running").arg(this->applicationDisplayName()));
	this->trayIco->setContextMenu(this->trayMenu);

	this->manager = new ToolManager(this);
	connect(this->manager, &ToolManager::errorOccured,
			this, &App::errorOccured);
}

int main(int argc, char *argv[])
{
	App a(argc, argv);
	return a.exec();
}
