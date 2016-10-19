#ifndef PEERSDIALOG_H
#define PEERSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class PeersDialog;
}

class PeersDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PeersDialog(const QString &title, QWidget *parent = nullptr);
	~PeersDialog();

public slots:
	void reloadPeers(QStandardItemModel *model);
	void popup();

signals:
	void reloadTriggered();

private:
	Ui::PeersDialog *ui;
	QStandardItemModel *model;
};

#endif // PEERSDIALOG_H
