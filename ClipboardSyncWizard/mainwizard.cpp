#include "mainwizard.h"
#include <qapplication.h>

#include "intropage.h"
#include "serversetuppage.h"
#include "clientsetuppage.h"

const QString MainWizard::ModeField("mode*");
const QString MainWizard::ServerNameField("name*");
const QString MainWizard::ServerPortField("port");
const QString MainWizard::ServerSecureField("secure");
const QString MainWizard::ServerLocalField("local");

MainWizard::MainWizard(QWidget *parent) :
	QWizard(parent, Qt::WindowCloseButtonHint)
{
	this->setWindowTitle(QApplication::applicationDisplayName() +
						 tr(" — Setup Wizard"));

	//this->setOption(QWizard::DisabledBackButtonOnLastPage);

	this->setPage(IntroPageId, new IntroPage(this));
	this->setPage(ServerSetupPageId, new ServerSetupPage(this));
	this->setPage(ClientSetupPageId, new ClientSetupPage(this));
	this->setStartId(IntroPageId);
}
