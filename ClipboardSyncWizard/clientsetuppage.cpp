#include "clientsetuppage.h"
#include "ui_clientsetuppage.h"
#include <QRegularExpressionValidator>

ClientSetupPage::ClientSetupPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ClientSetupPage),
	fType(QSsl::Pem)
{
	ui->setupUi(this);
	this->ui->serverADdressLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(R"__(^[a-z0-9](?:[a-z0-9\-\.]*[a-z0-9])?:(?:[1-9]|[1-8][0-9]|9[0-9]|[1-8][0-9]{2}|9[0-8][0-9]|99[0-9]|[1-8][0-9]{3}|9[0-8][0-9]{2}|99[0-8][0-9]|999[0-9]|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$)__"),
																									 QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption | QRegularExpression::OptimizeOnFirstUsageOption),
													  this->ui->serverADdressLineEdit));

	this->registerField(REQUIRED_FIELD(MainWizard::ClientNameField), this->ui->clientNameLineEdit);
	this->registerField(REQUIRED_FIELD(MainWizard::ClientUrlField), this->ui->serverADdressLineEdit);
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
	if(this->field(MainWizard::ModeField) == MainWizard::ServerMode) {
		this->ui->clientNameLineEdit->setText(this->field(MainWizard::ServerNameField).toString());
		this->ui->clientNameLabel->setEnabled(false);
		this->ui->clientNameLineEdit->setEnabled(false);
		this->ui->serverADdressLineEdit->setText(tr("Local Server"));
		this->ui->serverADdressLabel->setEnabled(false);
		this->ui->serverADdressLineEdit->setEnabled(false);
		this->ui->authenticationLineEdit->setText(this->field(MainWizard::ServerAuthPassField).toString());
		this->ui->authenticationCheckBox->setChecked(!this->ui->authenticationLineEdit->text().isEmpty());
		this->ui->authenticationCheckBox->setEnabled(false);
		this->ui->authenticationLineEdit->setEnabled(false);
		if(!this->field(MainWizard::ServerCertPathField).toString().isEmpty()) {
			this->ui->sBoxAll->setChecked(true);
			this->ui->sBoxUnsecure->setEnabled(false);
		} else {
			this->ui->sBoxAll->setEnabled(false);
			this->ui->sBoxCustom->setEnabled(false);
			this->ui->sBoxSystem->setEnabled(false);
		}
	}
}

void ClientSetupPage::cleanupPage()
{
	this->ui->clientNameLineEdit->clear();
	this->ui->clientNameLabel->setEnabled(true);
	this->ui->clientNameLineEdit->setEnabled(true);
	this->ui->serverADdressLineEdit->clear();
	this->ui->serverADdressLabel->setEnabled(true);
	this->ui->serverADdressLineEdit->setEnabled(true);
	this->ui->authenticationLineEdit->clear();
	this->ui->authenticationCheckBox->setChecked(false);
	this->ui->authenticationCheckBox->setEnabled(true);
	this->ui->sBoxUnsecure->setChecked(true);
	this->ui->sBoxUnsecure->setEnabled(true);
	this->ui->sBoxAll->setEnabled(true);
	this->ui->sBoxCustom->setEnabled(true);
	this->ui->sBoxSystem->setEnabled(true);
	this->ui->customCertPathEdit->clear();
	this->ui->customCertBox->setCurrentIndex(0);
}

bool ClientSetupPage::validatePage()
{
	if(!this->ui->authenticationCheckBox->isChecked())
		this->ui->authenticationLineEdit->clear();
	return true;
}

bool ClientSetupPage::isComplete() const
{
	bool ok = !this->ui->clientNameLineEdit->text().isEmpty();
	if(this->field(MainWizard::ModeField) == MainWizard::ClientMode)
		ok = ok && this->ui->serverADdressLineEdit->hasAcceptableInput();
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

MainWizard::SecurityMode ClientSetupPage::securityMode() const
{
	if(this->ui->sBoxUnsecure->isChecked())
		return MainWizard::NoSecurity;
	else if(this->ui->sBoxAll->isChecked())
		return MainWizard::SecureAll;
	else if(this->ui->sBoxCustom->isChecked())
		return MainWizard::SecureCustom;
	else if(this->ui->sBoxSystem->isChecked())
		return MainWizard::SecureSystem;
	else
		Q_UNREACHABLE();
}

QSsl::EncodingFormat ClientSetupPage::format() const
{
	return this->fType;
}
