#include "peersdialog.h"
#include "ui_peersdialog.h"

PeersDialog::PeersDialog(const QString &title, QWidget *parent) :
	QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
	ui(new Ui::PeersDialog),
	model(nullptr)
{
	this->ui->setupUi(this);
	this->setWindowTitle(title);
	this->setAttribute(Qt::WA_DeleteOnClose);

	connect(this->ui->reloadButton, &QPushButton::clicked,
			this, &PeersDialog::reloadTriggered);

	this->ui->treeView->addAction(this->ui->actionDisconnect_Client);
	this->ui->closeButton->setDefaultAction(this->ui->actionDisconnect_Client);
}

PeersDialog::~PeersDialog()
{
	delete this->ui;
}

void PeersDialog::reloadPeers(QStandardItemModel *model)
{
	if(this->model)
		this->model->deleteLater();
	this->model = model;
	model->setParent(this);
	this->ui->treeView->setModel(model);
}

void PeersDialog::popup()
{
	this->showNormal();
	this->raise();
	this->activateWindow();
	qApp->alert(this);
}
