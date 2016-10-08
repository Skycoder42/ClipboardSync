#ifndef CLIENTSETUPPAGE_H
#define CLIENTSETUPPAGE_H

#include <QWizardPage>

namespace Ui {
	class ClientSetupPage;
}

class ClientSetupPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit ClientSetupPage(QWidget *parent = nullptr);
	~ClientSetupPage();

private:
	Ui::ClientSetupPage *ui;
};

#endif // CLIENTSETUPPAGE_H
