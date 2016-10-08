#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWizardPage>
#include "mainwizard.h"

namespace Ui {
	class IntroPage;
}

class IntroPage : public QWizardPage
{
	Q_OBJECT

	Q_PROPERTY(MainWizard::WizardMode nextMode READ nextMode NOTIFY nextModeChanged)

public:
	explicit IntroPage(QWidget *parent = nullptr);
	~IntroPage();

	void cleanupPage() override;
	bool isComplete() const override;
	int nextId() const override;

	MainWizard::WizardMode nextMode() const;

signals:
	void nextModeChanged(MainWizard::WizardMode nextMode);

private slots:
	void on_serverButton_clicked();
	void on_clientButton_clicked();

private:
	Ui::IntroPage *ui;
	MainWizard::WizardMode next;
};

#endif // INTROPAGE_H
