#ifndef FINALPAGE_H
#define FINALPAGE_H

#include <QWizardPage>
#include "mainwizard.h"

namespace Ui {
class FinalPage;
}

class FinalPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit FinalPage(QWidget *parent = nullptr);
	~FinalPage();

	void initializePage() override;
	void cleanupPage() override;
	bool validatePage() override;
	bool isComplete() const override;
	int nextId() const override;

private slots:
	void reloadText(bool showPasswords);
	void doCreate(int which);

private:
	Ui::FinalPage *ui;

};

#endif // FINALPAGE_H
