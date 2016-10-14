#include "finalpage.h"
#include "ui_finalpage.h"

FinalPage::FinalPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::FinalPage)
{
	ui->setupUi(this);

	connect(this->ui->showPassBox, &QCheckBox::toggled,
			this, &FinalPage::reloadText);

	this->setButtonText(QWizard::CustomButton1, tr("Create Instances"));
}

FinalPage::~FinalPage()
{
	delete ui;
}

void FinalPage::initializePage()
{
	connect(this->wizard(), &QWizard::customButtonClicked,
			this, &FinalPage::doCreate);
	this->wizard()->setOption(QWizard::HaveCustomButton1, true);
	this->reloadText(false);
}

void FinalPage::cleanupPage()
{
	disconnect(this->wizard(), &QWizard::customButtonClicked,
			   this, &FinalPage::doCreate);
	this->ui->textBrowser->clear();
	this->wizard()->setOption(QWizard::HaveCustomButton1, false);
}

bool FinalPage::validatePage()
{
	return true;
}

bool FinalPage::isComplete() const
{
	return false;
}

int FinalPage::nextId() const
{
	return MainWizard::NoNextPage;
}

void FinalPage::reloadText(bool showPasswords)
{
	QString detailsText;

	if(this->wizard()->field(MainWizard::ModeField) == MainWizard::ServerMode) {
		detailsText += tr("<p><u>Server:</u><br/>");

		detailsText += tr("&nbsp;– Name: <i>%1</i><br/>").arg(this->wizard()->field(MainWizard::ServerNameField).toString());

		auto port = this->wizard()->field(MainWizard::ServerPortField).toInt();
		detailsText += tr("&nbsp;– Port: <i>%1</i><br/>").arg(port == 0 ? tr("Automatic Port") : QString::number(port));

		auto pass = this->wizard()->field(MainWizard::ServerAuthPassField).toString();
		if(!pass.isEmpty()) {
			if(showPasswords)
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(pass);
			else
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(tr("*").repeated(pass.size()));
		} else
			detailsText += tr("<font color=\"red\">&nbsp;– No Authentication</font><br/>");

		auto certPath = this->wizard()->field(MainWizard::ServerCertPathField).toString();
		if(!certPath.isEmpty())
			detailsText += tr("&nbsp;– Certificate: <i>%1</i><br/>").arg(certPath);
		else
			detailsText += tr("<font color=\"red\">&nbsp;– Unsecure Connection</font><br/>");

		if(this->wizard()->field(MainWizard::ServerLocalField).toBool())
			detailsText += tr("&nbsp;– Accept local network clients only<br/>");
		else
			detailsText += tr("&nbsp;– Accept all clients<br/>");

		detailsText += QStringLiteral("</p>");
	}

	if(!this->wizard()->field(MainWizard::ClientNameField).toString().isEmpty()) {
		detailsText += tr("<p><u>Client:</u><br/>");

		detailsText += tr("&nbsp;– Name: <i>%1</i><br/>").arg(this->wizard()->field(MainWizard::ClientNameField).toString());
		detailsText += tr("&nbsp;– Server Identity: <i>%1</i><br/>").arg(this->wizard()->field(MainWizard::ClientUrlField).toString());

		auto pass = this->wizard()->field(MainWizard::ClientAuthPassField).toString();
		if(!pass.isEmpty()) {
			if(showPasswords)
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(pass);
			else
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(tr("*").repeated(pass.size()));
		} else
			detailsText += tr("<font color=\"red\">&nbsp;– No Authentication</font><br/>");

		switch(this->wizard()->field(MainWizard::ClientSecurityField).toInt()) {
		case MainWizard::NoSecurity:
			detailsText += tr("<font color=\"red\">&nbsp;– Unsecure Connection</font><br/>");
			break;
		case MainWizard::SecureAll:
			detailsText += tr("<font color=\"orange\">&nbsp;– Secure Connection - Accept any server</font><br/>");
			break;
		case MainWizard::SecureCustom:
			detailsText += tr("&nbsp;– Secure Connection - Accept <i>%1</i> (<b>%2</b> Format)<br/>")
						   .arg(this->wizard()->field(MainWizard::ClientCertPathField).toString())
						   .arg(this->wizard()->field(MainWizard::ClientCertFormatField).toInt() == 0 ? tr("PEM") : tr("DER"));
			break;
		case MainWizard::SecureSystem:
			detailsText += tr("&nbsp;– Secure Connection - Accept system authorities<br/>");
			break;
		}

		detailsText += QStringLiteral("</p>");
	}

	this->ui->textBrowser->setHtml(detailsText);
}

void FinalPage::doCreate(int which)
{
	if(which == QWizard::CustomButton1) {
		this->wizard()->setOption(QWizard::DisabledBackButtonOnLastPage);
		this->wizard()->button(QWizard::CancelButton)->setEnabled(false);
		this->wizard()->button(QWizard::CustomButton1)->setEnabled(false);
	}
}
