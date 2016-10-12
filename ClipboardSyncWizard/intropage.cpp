#include "intropage.h"
#include "ui_intropage.h"

IntroPage::IntroPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::IntroPage),
	next(MainWizard::InvalidMode)
{
	ui->setupUi(this);

	this->registerField(REQUIRED_FIELD(MainWizard::ModeField),
						this,
						"nextMode",
						SIGNAL(nextModeChanged(MainWizard::WizardMode)));
}

IntroPage::~IntroPage()
{
	delete ui;
}

void IntroPage::cleanupPage()
{
	this->next = MainWizard::InvalidMode;
	emit nextModeChanged(this->next);
	emit completeChanged();
}

bool IntroPage::isComplete() const
{
	return this->next != MainWizard::InvalidMode;
}

int IntroPage::nextId() const
{
	switch (this->next) {
	case MainWizard::ServerMode:
		return MainWizard::ServerSetupPageId;
	case MainWizard::ClientMode:
		return MainWizard::ClientSetupPageId;
	default:
		return MainWizard::ServerSetupPageId;
	}
}

MainWizard::WizardMode IntroPage::nextMode() const
{
	return this->next;
}

void IntroPage::on_serverButton_clicked()
{
	this->ui->serverButton->setChecked(true);
	this->ui->clientButton->setChecked(false);
	this->next = MainWizard::ServerMode;
	emit nextModeChanged(this->next);
	emit completeChanged();
	this->wizard()->next();
}

void IntroPage::on_clientButton_clicked()
{
	this->ui->clientButton->setChecked(true);
	this->ui->serverButton->setChecked(false);
	this->next = MainWizard::ClientMode;
	emit nextModeChanged(this->next);
	emit completeChanged();
	this->wizard()->next();
}
