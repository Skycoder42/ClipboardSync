#ifndef FINALPAGE_H
#define FINALPAGE_H

#include <QWizardPage>
#include <QProgressDialog>
#include "mainwizard.h"
#include "toolmanager.h"

namespace Ui {
class FinalPage;
}

class FinalPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit FinalPage(ToolManager *manager, QWidget *parent = nullptr);
	~FinalPage();

	void initializePage() override;
	void cleanupPage() override;
	bool validatePage() override;
	bool isComplete() const override;
	int nextId() const override;

private slots:
	void reloadText(bool showPasswords);
	void doCreate(int which);

	void errorOccured(bool isServer, const QString &name, const QString &error);
	void serverCreated(const QString &name, quint16 port, const QStringList &localAddresses, const QString &remoteAddress);

private:
	Ui::FinalPage *ui;
	ToolManager *manager;

	QProgressDialog *createIndicator;
	bool complete;

	void createServer();
};

#endif // FINALPAGE_H
