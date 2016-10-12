#include "serversetuppage.h"
#include "ui_serversetuppage.h"
#include <QSslCertificate>
#include <QSslKey>
#include <dialogmaster.h>

ServerSetupPage::ServerSetupPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ServerSetupPage)
{
	ui->setupUi(this);

	this->registerField(MainWizard::ServerNameField, this->ui->serverNameLineEdit);
	this->registerField(MainWizard::ServerPortField, this->ui->serverPortSpinBox);
	this->registerField(MainWizard::ServerAuthPassField, this->ui->authenticationLineEdit);
	this->registerField(MainWizard::ServerCertPathField, this->ui->secureConnectionPathEdit);
	this->registerField(MainWizard::ServerCertPassField, this->ui->secureConnectionPassphraseEdit);
	this->registerField(MainWizard::ServerLocalField, this->ui->localhostOnlyCheckBox);

	connect(this->ui->serverNameLineEdit, &QLineEdit::textChanged,
			this, &ServerSetupPage::completeChanged);
	connect(this->ui->authenticationCheckBox, &QCheckBox::toggled,
			this, &ServerSetupPage::completeChanged);
	connect(this->ui->authenticationLineEdit, &QLineEdit::textChanged,
			this, &ServerSetupPage::completeChanged);
	connect(this->ui->secureConnectionCheckBox, &QCheckBox::toggled,
			this, &ServerSetupPage::completeChanged);
	connect(this->ui->secureConnectionPathEdit, &QPathEdit::pathChanged,
			this, &ServerSetupPage::completeChanged);
	connect(this->ui->secureConnectionPassphraseEdit, &QLineEdit::textChanged,
			this, &ServerSetupPage::completeChanged);
}

ServerSetupPage::~ServerSetupPage()
{
	delete ui;
}

bool ServerSetupPage::validatePage()
{
	if(this->ui->secureConnectionCheckBox->isChecked()) {
		QFile file(this->ui->secureConnectionPathEdit->path());
		if(!file.open(QIODevice::ReadOnly)) {
			DialogMaster::warning(this,
								  tr("The wizard failed to open the file! Check if it exists and make shure no other programs are currently using it!"),
								  tr("Unable to load PKCS#12-File!"));
			return false;
		}
		QSslKey key;
		QSslCertificate cert;
		QList<QSslCertificate> caCertificates;
		auto ok = QSslCertificate::importPkcs12(&file,
												&key,
												&cert,
												&caCertificates,
												this->ui->secureConnectionPassphraseEdit->text().toUtf8());
		file.close();
		if(!ok)
			DialogMaster::warning(this,
								  tr("The wizard was not able to verify the files contents. Either the file is corrupted or your passphrase is wrong!"),
								  tr("Unable to load PKCS#12-File!"));
		return ok;
	} else
		return true;
}

bool ServerSetupPage::isComplete() const
{
	bool ok = !this->ui->serverNameLineEdit->text().isEmpty();
	if(this->ui->authenticationCheckBox->isChecked())
		ok = ok && !this->ui->authenticationLineEdit->text().isEmpty();
	if(this->ui->secureConnectionCheckBox->isChecked()) {
		ok = ok && !this->ui->secureConnectionPathEdit->path().isEmpty();
		ok = ok && !this->ui->secureConnectionPassphraseEdit->text().isEmpty();
	}
	return ok;
}

void ServerSetupPage::cleanupPage()
{
	this->ui->serverNameLineEdit->clear();
	this->ui->serverPortSpinBox->setValue(-1);
	this->ui->secureConnectionCheckBox->setChecked(false);
	this->ui->localhostOnlyCheckBox->setChecked(false);
	this->ui->createClientCheckBox->setChecked(true);
}

int ServerSetupPage::nextId() const
{
	return this->ui->createClientCheckBox->isChecked() ?
				MainWizard::ClientSetupPageId :
				MainWizard::FinalPageId;
}
