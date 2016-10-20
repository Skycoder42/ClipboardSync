#include "finalpage.h"
#include "ui_finalpage.h"
#include "dialogmaster.h"

FinalPage::FinalPage(ToolManager *manager, QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::FinalPage),
	manager(manager),
	createIndicator(DialogMaster::createProgress(this, tr("Creating Instances. Please wait…"), 0, 0, false)),
	sComp(false),
	cComp(false),
	complete(false),
	error()
{
	this->ui->setupUi(this);
	this->createIndicator->cancel();

	connect(this->ui->showPassBox, &QCheckBox::toggled,
			this, &FinalPage::reloadText);

	this->setButtonText(QWizard::CustomButton1, tr("Create Instances"));

	connect(this->manager, &ToolManager::showMessage,
			this, &FinalPage::errorOccured);
	connect(this->manager, &ToolManager::serverStatusLoaded,
			this, &FinalPage::serverCreated);
}

FinalPage::~FinalPage()
{
	delete this->ui;
}

void FinalPage::initializePage()
{
	connect(this->wizard(), &QWizard::customButtonClicked,
			this, &FinalPage::doCreate);
	this->wizard()->setOption(QWizard::HaveCustomButton1, true);
	this->reloadText();
}

void FinalPage::cleanupPage()
{
	disconnect(this->wizard(), &QWizard::customButtonClicked,
			   this, &FinalPage::doCreate);
	this->ui->textBrowser->clear();
	this->sComp = false;
	this->cComp = false;
	this->complete = false;
	this->error.clear();
	this->wizard()->setOption(QWizard::HaveCustomButton1, false);
}

bool FinalPage::validatePage()
{
	return true;
}

bool FinalPage::isComplete() const
{
	return this->complete;
}

int FinalPage::nextId() const
{
	return MainWizard::NoNextPage;
}

void FinalPage::reloadText()
{
	QString detailsText;

	if(this->field(MainWizard::ModeField) == MainWizard::ServerMode) {
		detailsText += tr("<p><u>Server:</u><br/>");

		detailsText += tr("&nbsp;– Name: <i>%1</i><br/>").arg(this->field(MainWizard::ServerNameField).toString());

		auto port = this->field(MainWizard::ServerPortField).toInt();
		detailsText += tr("&nbsp;– Port: <i>%1</i><br/>").arg(port == 0 ? tr("Automatic Port") : QString::number(port));

		auto pass = this->field(MainWizard::ServerAuthPassField).toString();
		if(!pass.isEmpty()) {
			if(this->ui->showPassBox->isChecked())
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(pass);
			else
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(tr("*").repeated(pass.size()));
		} else
			detailsText += tr("<font color=\"red\">&nbsp;– No Authentication</font><br/>");

		auto certPath = this->field(MainWizard::ServerCertPathField).toString();
		if(!certPath.isEmpty())
			detailsText += tr("&nbsp;– Certificate: <i>%1</i><br/>").arg(certPath);
		else
			detailsText += tr("<font color=\"red\">&nbsp;– Unsecure Connection</font><br/>");

		if(this->field(MainWizard::ServerLocalField).toBool())
			detailsText += tr("&nbsp;– Accept local network clients only");
		else
			detailsText += tr("&nbsp;– Accept all clients");

		if(this->sComp)
			detailsText += QStringLiteral("<br/><br/><font color=\"#088A08\">Server successfully stared!</font>");

		detailsText += QStringLiteral("</p>");
	}

	if(!this->field(MainWizard::ClientNameField).toString().isEmpty()) {
		detailsText += tr("<p><u>Client:</u><br/>");

		detailsText += tr("&nbsp;– Name: <i>%1</i><br/>").arg(this->field(MainWizard::ClientNameField).toString());
		detailsText += tr("&nbsp;– Server Identity: <i>%1</i><br/>").arg(this->field(MainWizard::ClientUrlField).toString());

		auto pass = this->field(MainWizard::ClientAuthPassField).toString();
		if(!pass.isEmpty()) {
			if(this->ui->showPassBox->isChecked())
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(pass);
			else
				detailsText += tr("&nbsp;– Authentication: <i>%1</i><br/>").arg(tr("*").repeated(pass.size()));
		} else
			detailsText += tr("<font color=\"red\">&nbsp;– No Authentication</font><br/>");

		switch(this->field(MainWizard::ClientSecurityField).toInt()) {
		case MainWizard::NoSecurity:
			detailsText += tr("<font color=\"red\">&nbsp;– Unsecure Connection</font>");
			break;
		case MainWizard::SecureAll:
			detailsText += tr("<font color=\"orange\">&nbsp;– Secure Connection - Accept any server</font>");
			break;
		case MainWizard::SecureCustom:
			detailsText += tr("&nbsp;– Secure Connection - Accept <i>%1</i> (<b>%2</b> Format)")
						   .arg(this->field(MainWizard::ClientCertPathField).toString())
						   .arg(this->field(MainWizard::ClientCertFormatField).toInt() == 0 ? tr("PEM") : tr("DER"));
			break;
		case MainWizard::SecureSystem:
			detailsText += tr("&nbsp;– Secure Connection - Accept system authorities");
			break;
		}

		if(this->cComp)
			detailsText += QStringLiteral("<br/><br/><font color=\"#088A08\">Client successfully stared!</font>");

		detailsText += QStringLiteral("</p>");
	}

	if(!this->error.isNull())
		detailsText += this->error;

	this->ui->textBrowser->setHtml(detailsText);
	if(this->complete) {
		this->ui->textBrowser->moveCursor(QTextCursor::End);
		this->ui->textBrowser->ensureCursorVisible();
	}
}

void FinalPage::doCreate(int which)
{
	if(which == QWizard::CustomButton1) {
		this->wizard()->setOption(QWizard::DisabledBackButtonOnLastPage);
		this->wizard()->button(QWizard::CancelButton)->setEnabled(false);
		this->wizard()->button(QWizard::CustomButton1)->setEnabled(false);

		this->createIndicator->open();
		if(this->field(MainWizard::ModeField) == MainWizard::ServerMode)
			this->createServer();
	}
}

void FinalPage::errorOccured(QtMsgType type, const QString &title, const QString &message)
{
	if(type == QtMsgType::QtCriticalMsg) {
		this->createIndicator->close();
		this->wizard()->setOption(QWizard::DisabledBackButtonOnLastPage, false);
		this->wizard()->button(QWizard::CancelButton)->setEnabled(true);
		this->wizard()->button(QWizard::CustomButton1)->setEnabled(true);

		this->error = tr("<p><font color=\"#B40404\"><u>%1</u><br/>%2</font></p>")
				.arg(title)
				.arg(message);
		this->reloadText();
	}
}

void FinalPage::serverCreated()
{
	this->createIndicator->close();
	this->sComp = true;
	this->complete = true;
	this->reloadText();
	emit completeChanged();
}

void FinalPage::createServer()
{
	this->manager->createServer(this->field(MainWizard::ServerNameField).toString(),
								this->field(MainWizard::ServerPortField).toInt(),
								this->field(MainWizard::ServerAuthPassField).toString(),
								this->field(MainWizard::ServerCertPathField).toString(),
								this->field(MainWizard::ServerCertPassField).toString(),
								this->field(MainWizard::ServerLocalField).toBool());
}
