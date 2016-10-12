#ifndef MAINWIZARD_H
#define MAINWIZARD_H

#include <QWizard>

class MainWizard : public QWizard
{
	Q_OBJECT

public:
	enum PageIds {
		IntroPageId,
		ServerSetupPageId,
		ClientSetupPageId,
		FinalPageId,

		NoNextPage = -1
	};
	Q_ENUM(PageIds)

	enum WizardMode {
		InvalidMode,
		ClientMode,
		ServerMode
	};
	Q_ENUM(WizardMode)

	static const QString ModeField;

	static const QString ServerNameField;
	static const QString ServerAuthPassField;
	static const QString ServerPortField;
	static const QString ServerCertPathField;
	static const QString ServerCertPassField;
	static const QString ServerLocalField;

	static const QString ClientNameField;
	static const QString ClientAuthPassField;
	static const QString ClientUrlField;
	static const QString ClientSecurityField;
	static const QString ClientCertPathField;
	static const QString ClientCertFormatField;

	explicit MainWizard(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // MAINWIZARD_H
