#include "mainwizard.h"
#include <qapplication.h>

#include "intropage.h"
#include "serversetuppage.h"
#include "clientsetuppage.h"
#include "finalpage.h"

const QString MainWizard::ModeField(QStringLiteral("mode"));
const QString MainWizard::ServerNameField(QStringLiteral("name"));
const QString MainWizard::ServerPortField(QStringLiteral("port"));
const QString MainWizard::ServerAuthPassField(QStringLiteral("serverAuth"));
const QString MainWizard::ServerCertPathField(QStringLiteral("securePath"));
const QString MainWizard::ServerCertPassField(QStringLiteral("securePass"));
const QString MainWizard::ServerLocalField(QStringLiteral("local"));
const QString MainWizard::ClientNameField(QStringLiteral("origin"));
const QString MainWizard::ClientUrlField(QStringLiteral("url"));
const QString MainWizard::ClientAuthPassField(QStringLiteral("clientAuth"));
const QString MainWizard::ClientSecurityField(QStringLiteral("secureMode"));
const QString MainWizard::ClientCertPathField(QStringLiteral("customPath"));
const QString MainWizard::ClientCertFormatField(QStringLiteral("customFormat"));

MainWizard::MainWizard(ToolManager *manager, QWidget *parent) :
	QWizard(parent, Qt::WindowCloseButtonHint)
{
	this->setWindowModality(Qt::NonModal);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowTitle(QApplication::applicationDisplayName() +
						 tr(" — Setup Wizard"));

	this->setDefaultProperty("QPathEdit", "path", SIGNAL(pathChanged(QString)));

	this->setPage(IntroPageId, new IntroPage(this));
	this->setPage(ServerSetupPageId, new ServerSetupPage(this));
	this->setPage(ClientSetupPageId, new ClientSetupPage(this));
	this->setPage(FinalPageId, new FinalPage(manager, this));
	this->setStartId(IntroPageId);
}

MainWizard *MainWizard::createInstance(ToolManager *manager, QWidget *parent)
{
	auto wizard = new MainWizard(manager, parent);
	wizard->show();
	wizard->raise();
	wizard->activateWindow();
	qApp->alert(wizard);
	return wizard;
}
