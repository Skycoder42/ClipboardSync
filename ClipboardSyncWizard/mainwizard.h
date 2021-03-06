#ifndef MAINWIZARD_H
#define MAINWIZARD_H

#include <QWizard>
class ToolManager;

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

	enum SecurityMode {
		NoSecurity,
		SecureAll,
		SecureCustom,
		SecureSystem
	};
	Q_ENUM(SecurityMode)

	static const QString ModeField;

	static const QString ServerNameField;
	static const QString ServerPortField;
	static const QString ServerAuthPassField;
	static const QString ServerCertPathField;
	static const QString ServerCertPassField;
	static const QString ServerLocalField;

	static const QString ClientNameField;
	static const QString ClientUrlField;
	static const QString ClientAuthPassField;
	static const QString ClientSecurityField;
	static const QString ClientCertPathField;
	static const QString ClientCertFormatField;

	explicit MainWizard(ToolManager *manager, QWidget *parent = nullptr);

	static MainWizard *createInstance(ToolManager *manager, QWidget *parent = nullptr);
};

#define REQUIRED_FIELD(x) (x + QLatin1Char('*'))

#endif // MAINWIZARD_H
