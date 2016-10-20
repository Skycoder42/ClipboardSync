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
	void serverCreated();

private:
	Ui::FinalPage *ui;
	ToolManager *manager;

	QProgressDialog *createIndicator;
	bool sComp;
	bool cComp;
	bool complete;
	QString error;

	void createServer();
};

#endif // FINALPAGE_H
