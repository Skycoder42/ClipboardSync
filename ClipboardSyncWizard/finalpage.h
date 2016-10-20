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
	void reloadText();
	void doCreate(int which);

	void errorOccured(QtMsgType type, const QString &title, const QString &message);
	void serverCreated(const QString &, const ToolManager::ServerInfo &info);
	void clientCreated(const QString &, const QString &serverName);

private:
	Ui::FinalPage *ui;
	ToolManager *manager;

	QProgressDialog *createIndicator;
	int rServerPort;
	bool sComp;
	bool cComp;
	bool complete;
	QString svrName;
	QString error;

	void createServer();
	void createClient(quint16 port = 0);
};

#endif // FINALPAGE_H
