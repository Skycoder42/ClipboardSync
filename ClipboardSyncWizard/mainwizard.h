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
	static const QString ServerPortField;
	static const QString ServerAuthPassField;
	static const QString ServerCertPathField;
	static const QString ServerCertPassField;
	static const QString ServerLocalField;

	explicit MainWizard(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // MAINWIZARD_H
