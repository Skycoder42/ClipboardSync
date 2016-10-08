#include "serversetuppage.h"
#include "ui_serversetuppage.h"

ServerSetupPage::ServerSetupPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ServerSetupPage)
{
	ui->setupUi(this);

	this->registerField(MainWizard::ServerNameField, this->ui->serverNameLineEdit);
	this->registerField(MainWizard::ServerPortField, this->ui->serverPortSpinBox);
	this->registerField(MainWizard::ServerSecureField, this->ui->secureConnectionCheckBox);
	this->registerField(MainWizard::ServerLocalField, this->ui->localhostOnlyCheckBox);
}

ServerSetupPage::~ServerSetupPage()
{
	delete ui;
}

void ServerSetupPage::cleanupPage()
{
	this->ui->serverNameLineEdit->clear();
	this->ui->serverPortSpinBox->setValue(-1);
	this->ui->secureConnectionCheckBox->setChecked(false);
	this->ui->localhostOnlyCheckBox->setChecked(false);
}

int ServerSetupPage::nextId() const
{
	return MainWizard::NoNextPage;
}
