#ifndef FINALPAGE_H
#define FINALPAGE_H

#include <QWizardPage>

namespace Ui {
class FinalPage;
}

class FinalPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit FinalPage(QWidget *parent = nullptr);
	~FinalPage();

private:
	Ui::FinalPage *ui;
};

#endif // FINALPAGE_H
