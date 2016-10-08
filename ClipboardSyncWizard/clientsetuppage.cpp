#include "clientsetuppage.h"
#include "ui_clientsetuppage.h"

ClientSetupPage::ClientSetupPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ClientSetupPage)
{
	ui->setupUi(this);
}

ClientSetupPage::~ClientSetupPage()
{
	delete ui;
}
