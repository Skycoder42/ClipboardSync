#include "clientsetuppage.h"
#include "ui_clientsetuppage.h"
#include <QRegularExpressionValidator>
#include <QSslCertificate>
#include <dialogmaster.h>
#include "app.h"

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
		this->ui->clientNameLineEdit->setText(this->field(MainWizard::ServerNameField).toString() + QStringLiteral("_Client"));
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
	auto name = this->ui->clientNameLineEdit->text();
	if(!qApp->testNameUnique(name) || this->field(MainWizard::ServerNameField) == name) {
		DialogMaster::warning(this,
							  tr("The name \"%1\" is aleady in use! Please choose another one.").arg(name),
							  tr("Name already in use!"));
		return false;
	}

	if(this->ui->sBoxCustom->isChecked()) {
		do {//create "break" context
			if(this->ui->customCertBox->currentIndex() != 2) {//Auto or PEM
				auto certs = QSslCertificate::fromPath(this->ui->customCertPathEdit->path(), QSsl::Pem);
				if(!certs.isEmpty()) {
					this->fType = QSsl::Pem;
					emit formatChanged(QSsl::Pem);
					break;
				}
			}

			if(this->ui->customCertBox->currentIndex() != 1) {//Auto or DER
				auto certs = QSslCertificate::fromPath(this->ui->customCertPathEdit->path(), QSsl::Der);
				if(!certs.isEmpty()) {
					this->fType = QSsl::Der;
					emit formatChanged(QSsl::Der);
					break;
				}
			}

			DialogMaster::warning(this,
								  tr("The application was not able to verify the certificate with the Format <i>%1</i>. "
									 "Either the format is wrong or the file is corrupted!")
								  .arg(this->ui->customCertBox->currentText()),
								  tr("Unable to verify certificate!"));
			return false;
		} while(false);
	}

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
