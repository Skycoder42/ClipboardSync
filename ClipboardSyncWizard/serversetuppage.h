#ifndef SERVERSETUPPAGE_H
#define SERVERSETUPPAGE_H

#include <QWizardPage>
#include "mainwizard.h"

namespace Ui {
	class ServerSetupPage;
}

class ServerSetupPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit ServerSetupPage(QWidget *parent = nullptr);
	~ServerSetupPage();

	void cleanupPage() override;
	int nextId() const override;

private:
	Ui::ServerSetupPage *ui;
};

#endif // SERVERSETUPPAGE_H
