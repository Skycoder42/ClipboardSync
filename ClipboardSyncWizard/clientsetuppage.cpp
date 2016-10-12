#include "clientsetuppage.h"
#include "ui_clientsetuppage.h"

ClientSetupPage::ClientSetupPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ClientSetupPage),
	fType(QSsl::Pem)
{
	ui->setupUi(this);

	this->registerField(MainWizard::ClientNameField, this->ui->clientNameLineEdit);
	this->registerField(MainWizard::ClientUrlField, this->ui->serverADdressLineEdit);
	this->registerField(MainWizard::ClientAuthPassField, this->ui->authenticationLineEdit);
	this->registerField(MainWizard::ClientSecurityField, this, "securityMode", SIGNAL(securityModeChanged(SecurityMode)));
	this->registerField(MainWizard::ClientCertPathField, this->ui->customCertPathEdit);
	this->registerField(MainWizard::ClientCertFormatField, this, "format", SIGNAL(formatChanged(QSsl::EncodingFormat)));

	connect(this->ui->clientNameLineEdit, &QLineEdit::textChanged,
			this, &ClientSetupPage::completeChanged);
	connect(this->ui->serverADdressLineEdit, &QLineEdit::textChanged,
			this, &ClientSetupPage::completeChanged);
	connect(this->ui->authenticationCheckBox, &QCheckBox::toggled,
			this, &ClientSetupPage::completeChanged);
	connect(this->ui->authenticationLineEdit, &QLineEdit::textChanged,
			this, &ClientSetupPage::completeChanged);
	connect(this->ui->sBoxCustom, &QRadioButton::toggled,
			this, &ClientSetupPage::completeChanged);
	connect(this->ui->customCertPathEdit, &QPathEdit::pathChanged,
			this, &ClientSetupPage::completeChanged);
}

ClientSetupPage::~ClientSetupPage()
{
	delete ui;
}

void ClientSetupPage::initializePage()
{

}

void ClientSetupPage::cleanupPage()
{

}

bool ClientSetupPage::validatePage()
{
	return true;
}

bool ClientSetupPage::isComplete() const
{
	bool ok = !this->ui->clientNameLineEdit->text().isEmpty() &&
			  this->ui->serverADdressLineEdit->hasAcceptableInput();
	if(this->ui->authenticationCheckBox->isChecked())
		ok = ok && !this->ui->authenticationLineEdit->text().isEmpty();
	if(this->ui->sBoxCustom->isChecked())
		ok = ok && !this->ui->customCertPathEdit->path().isEmpty();
	return ok;
}

int ClientSetupPage::nextId() const
{
	return MainWizard::FinalPageId;
}

ClientSetupPage::SecurityMode ClientSetupPage::securityMode() const
{
	if(this->ui->sBoxUnsecure->isChecked())
		return NoSecurity;
	else if(this->ui->sBoxAll->isChecked())
		return SecureAll;
	else if(this->ui->sBoxCustom->isChecked())
		return SecureCustom;
	else if(this->ui->sBoxSystem->isChecked())
		return SecureSystem;
	else
		Q_UNREACHABLE();
}

QSsl::EncodingFormat ClientSetupPage::format() const
{
	return this->fType;
}
