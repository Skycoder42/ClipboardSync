#ifndef CLIENTSETUPPAGE_H
#define CLIENTSETUPPAGE_H

#include <QWizardPage>
#include <QSsl>
#include "mainwizard.h"

namespace Ui {
	class ClientSetupPage;
}

class ClientSetupPage : public QWizardPage
{
	Q_OBJECT

	Q_PROPERTY(MainWizard::SecurityMode securityMode READ securityMode NOTIFY securityModeChanged)
	Q_PROPERTY(QSsl::EncodingFormat format READ format NOTIFY formatChanged)

public:

	explicit ClientSetupPage(QWidget *parent = nullptr);
	~ClientSetupPage();

	void initializePage() override;
	void cleanupPage() override;
	bool validatePage() override;
	bool isComplete() const override;
	int nextId() const override;

	MainWizard::SecurityMode securityMode() const;
	QSsl::EncodingFormat format() const;

signals:
	void securityModeChanged(MainWizard::SecurityMode securityMode);
	void formatChanged(QSsl::EncodingFormat format);

private:
	Ui::ClientSetupPage *ui;
	QSsl::EncodingFormat fType;
};

#endif // CLIENTSETUPPAGE_H
