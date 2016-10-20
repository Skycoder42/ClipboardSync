#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LogDialog(const QString &title, QWidget *parent = nullptr);
	~LogDialog();

public slots:
	void reloadLog(const QByteArray &log);
	void popup();

signals:
	void reloadTriggered();

private slots:
	void on_saveButton_clicked();

private:
	Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
