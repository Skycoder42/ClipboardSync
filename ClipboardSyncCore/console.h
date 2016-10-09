#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QPointer>
#include "clipboardsynccore_global.h"

class CLIPBOARDSYNCCORESHARED_EXPORT Console : public QThread
{
	Q_OBJECT

	Q_PROPERTY(bool autoExit READ autoExit WRITE setAutoExit)

public:
	explicit Console(QObject *parent = nullptr);
	~Console();

	void installAsMessageHandler();

	bool autoExit() const;

public slots:
	void writeMessage(bool isError, const QByteArray &message, bool lineFeed = true);

	void setAutoExit(bool autoExit);

signals:
	void commandReceived(const QByteArray &command);

protected:
	void run() override;

private:
	QFile *qOut;
	QFile *qErr;
	bool autoQuit;

	static QPointer<Console> consoleInstance;
	static void consoleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // CONSOLE_H
