#include "logdialog.h"
#include "ui_logdialog.h"
#include <QSyntaxHighlighter>
#include <QBrush>
#include <QColor>
#include <QTextDocumentWriter>
#include <QDesktopServices>
#include <dialogmaster.h>

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
	QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
	ui(new Ui::LogDialog)
{
	this->ui->setupUi(this);
	this->setWindowTitle(title);
	this->setAttribute(Qt::WA_DeleteOnClose);

	connect(this->ui->reloadButton, &QPushButton::clicked,
			this, &LogDialog::reloadTriggered);

	new KeyHighlighter(this->ui->textBrowser->document());
}

LogDialog::~LogDialog()
{
	delete this->ui;
}

void LogDialog::reloadLog(const QByteArray &log)
{
	this->ui->textBrowser->setPlainText(QString::fromUtf8(log));
	this->ui->textBrowser->moveCursor(QTextCursor::End);
	this->ui->textBrowser->ensureCursorVisible();
}

void LogDialog::popup()
{
	this->showNormal();
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

void LogDialog::on_saveButton_clicked()
{
	QString selected;

	QStringList filters = {//TODO use mimetypes and default formats?
		tr("OpenDocument Format (*.odf)"),
		tr("Plain Text (*.txt)"),
		tr("HyperText Markup Language (*.html *.htm)")
	};

	auto name = DialogMaster::getSaveFileName(this,
											  tr("Save Debug Log"),
											  QString(),
											  filters.join(QStringLiteral(";;")),
											  &selected);
	if(!name.isEmpty()) {
		auto formatIndex = filters.indexOf(selected);
		QByteArray format;
		switch (formatIndex) {
		case 0:
			format = "ODF";
			break;
		case 1:
		default:
			format = "plaintext";
			break;
		case 2:
			format = "HTML";
			break;
		}
		QTextDocumentWriter writer(name, format);
		if(writer.write(this->ui->textBrowser->document()))
			QDesktopServices::openUrl(QUrl::fromLocalFile(name));
		else
			DialogMaster::warning(this, tr("Failed to save document to file!"));
	}
}
