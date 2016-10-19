#include "logdialog.h"
#include "ui_logdialog.h"
#include <QSyntaxHighlighter>
#include <QBrush>
#include <QColor>
#include <QWindow>

class KeyHighlighter : public QSyntaxHighlighter
{
public:
	KeyHighlighter(QTextDocument *parent);

protected:
	void highlightBlock(const QString &text) override;

private:
	QTextCharFormat debugFormat;
	QTextCharFormat warningFormat;
	QTextCharFormat criticalFormat;
	QTextCharFormat fatalFormat;
};

LogDialog::LogDialog(const QString &title, QWidget *parent) :
	QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint),
	ui(new Ui::LogDialog)
{
	this->ui->setupUi(this);
	this->setWindowTitle(title);
	this->setAttribute(Qt::WA_DeleteOnClose);

	new KeyHighlighter(this->ui->textBrowser->document());

	connect(this->ui->reloadButton, &QPushButton::clicked,
			this, &LogDialog::reloadTriggered);
}

LogDialog::~LogDialog()
{
	delete this->ui;
}

void LogDialog::reloadLog(const QByteArray &log)
{
	this->ui->textBrowser->setPlainText(QString::fromUtf8(log));
}

void LogDialog::popup()
{
	this->show();
	if(this->windowState().testFlag(Qt::WindowMinimized)) {
		if(this->windowState().testFlag(Qt::WindowMaximized))
			this->showMaximized();
		else
			this->showNormal();
	}
	this->raise();
	this->activateWindow();
	qApp->alert(this);
}



KeyHighlighter::KeyHighlighter(QTextDocument *parent) :
	QSyntaxHighlighter(parent),
	debugFormat(),
	warningFormat(),
	criticalFormat(),
	fatalFormat()
{
	this->debugFormat.setForeground(QColor("#5882FA"));
	this->warningFormat.setForeground(QColor("#FF8000"));
	this->criticalFormat.setForeground(QColor("#B40404"));
	this->fatalFormat.setForeground(QColor("#610B5E"));
}

void KeyHighlighter::highlightBlock(const QString &text)
{
	const QRegularExpression regex(QStringLiteral(R"__(^\[(Debug|Warning|Critical|Fatal)\]\s)__"),
								   QRegularExpression::MultilineOption | QRegularExpression::OptimizeOnFirstUsageOption);

	auto match = regex.match(text, 0);
	while(match.hasMatch()) {
		auto index = match.capturedStart();
		auto len = match.capturedLength();
		QTextCharFormat format;
		if(match.captured(1) == QStringLiteral("Debug"))
			this->setFormat(index, len, this->debugFormat);
		else if(match.captured(1) == QStringLiteral("Warning"))
			this->setFormat(index, len, this->warningFormat);
		else if(match.captured(1) == QStringLiteral("Critical"))
			this->setFormat(index, len, this->criticalFormat);
		else if(match.captured(1) == QStringLiteral("Fatal"))
			this->setFormat(index, len, this->fatalFormat);

		match = regex.match(text, index + len);
	}
}
