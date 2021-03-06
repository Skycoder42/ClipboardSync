#include "menumanager.h"
#include "app.h"
#include <dialogmaster.h>

MenuManager::MenuManager(QSystemTrayIcon *trayIco, QObject *parent) :
	QObject(parent),
	trayIco(trayIco),
	menu(new QMenu()),
	createEnabled(true),
	postServers(nullptr),
	postClients(nullptr),
	elements()
{
	this->trayIco->setToolTip(qApp->applicationDisplayName());
	this->trayIco->setContextMenu(this->menu.data());

	this->menu->addAction(tr("&Add Instance"), qApp, &App::showCreate);
	this->menu->addAction(tr("&Import Configuration"), this, [this](){
		this->setCreateEnabled(false);
		emit doImport();
	});
	this->menu->addSection(tr("Servers"));
	this->postServers = this->menu->addSection(tr("Clients"));
	this->postClients = this->menu->addSeparator();
	this->menu->addAction(tr("Start on &boot"))->setEnabled(false);//TODO ->setCheckable(true);
	this->menu->addAction(tr("&Quit"), qApp, &App::quit);
}

bool MenuManager::isCreateEnabled() const
{
	return this->createEnabled;
}

void MenuManager::completeImport()
{
	this->setCreateEnabled(true);
}

void MenuManager::addServer(const QString &name, bool fromStartup)
{
	auto menu = new QMenu(name, this->menu.data());

	menu->addAction(tr("&Synchronize"), this, [=](){
		emit performAction(name, ToolManager::Sync);
	});
	menu->addAction(tr("&Clear Clipboard Data"), this, [=](){
		emit performAction(name, ToolManager::Clear);
	});
	menu->addSeparator();
	menu->addAction(tr("Show S&tatus"), this, [=](){
		emit performAction(name, ToolManager::Status);
	});
	menu->addAction(tr("Show Connected Cl&ients"), this, [=](){
		emit performAction(name, ToolManager::Peers);
	});
	menu->addAction(tr("Show Server &Log"), this, [=](){
		emit performAction(name, ToolManager::Log);
	});
	menu->addSeparator();
	auto remAction = menu->addAction(tr("C&reate on startup"), this, [=](){
		emit performAction(name, ToolManager::ToggleAutoSave);
	});
	remAction->setCheckable(true);
	remAction->setChecked(fromStartup);
	menu->addAction(tr("E&xport Configuration"), this, [=](){
		emit performAction(name, ToolManager::Save);
	});
	menu->addAction(tr("&Quit Server"), this, [=](){
		emit performAction(name, ToolManager::Close);
	});

	this->menu->insertMenu(this->postServers, menu);
	this->elements.insert(name, menu);

	this->reloadTooltip();
}

void MenuManager::addClient(const QString &name, bool fromStartup)
{
	auto menu = new QMenu(name, this->menu.data());

	menu->addAction(tr("&Synchronize"), this, [=](){
		emit performAction(name, ToolManager::Sync);
	});
	menu->addAction(tr("&Clear Clipboard Data"), this, [=](){
		emit performAction(name, ToolManager::Clear);
	});
	menu->addAction(tr("Set Sync &Interval"), this, [=](){
		this->showSetSync(name);
	});
	menu->addSeparator();
	menu->addAction(tr("Show Client &Log"), this, [=](){
		emit performAction(name, ToolManager::Log);
	});
	menu->addSeparator();
	auto remAction = menu->addAction(tr("C&reate on startup"), this, [=](){
		emit performAction(name, ToolManager::ToggleAutoSave);
	});
	remAction->setCheckable(true);
	remAction->setChecked(fromStartup);
	menu->addAction(tr("E&xport Configuration"), this, [=](){
		emit performAction(name, ToolManager::Save);
	});
	menu->addAction(tr("&Quit Client"), this, [=](){
		emit performAction(name, ToolManager::Close);
	});

	this->menu->insertMenu(this->postClients, menu);
	this->elements.insert(name, menu);

	this->reloadTooltip();
}

void MenuManager::removeInstance(const QString &name)
{
	auto elem = this->elements.take(name);
	if(elem)
		elem->deleteLater();
	this->reloadTooltip();
}

void MenuManager::setCreateEnabled(bool createEnabled)
{
	if (this->createEnabled == createEnabled)
		return;

	this->createEnabled = createEnabled;
	this->menu->actions()[0]->setEnabled(createEnabled);
	this->menu->actions()[1]->setEnabled(createEnabled);
	emit createEnabledChanged(createEnabled);
}

void MenuManager::showSetSync(const QString &name)
{
	bool ok;
	auto interval = DialogMaster::getDouble(nullptr,
											tr("Enter the interval (in seconds) to check for an updated clipboard.\n"
											"To deactive periodic sync checks, simply enter 0:"),
											0.0,
											0.0,
											86400.0,
											tr("Set Sync Interval"),
											3,
											&ok);
	if(ok)
		emit setSyncInterval(name, interval * 1000);

}

void MenuManager::reloadTooltip()
{
	QStringList servers;
	QStringList clients;

	auto sepIndex = this->menu->actions().indexOf(this->postServers);
	for(auto it = this->elements.constBegin(); it != this->elements.constEnd(); it++) {
		if(this->menu->actions().indexOf(it.value()->menuAction()) < sepIndex)
			servers.append(it.key());
		else
			clients.append(it.key());
	}

	auto tooltip = qApp->applicationDisplayName();
	if(!servers.isEmpty()) {
		tooltip.append(tr("\nServers:"));
		foreach(auto server, servers)
			tooltip.append(tr("\n - ") + server);
	}
	if(!clients.isEmpty()) {
		tooltip.append(tr("\nClients:"));
		foreach(auto client, clients)
			tooltip.append(tr("\n - ") + client);
	}

	if(this->trayIco)
		this->trayIco->setToolTip(tooltip);
}
