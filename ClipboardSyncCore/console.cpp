#include "console.h"
#include <QTextStream>
#include <QCoreApplication>
#include <iostream>

QPointer<Console> Console::consoleInstance;

Console::Console(QObject *parent) :
	QThread(parent),
	qOut(new QFile(this)),
	qErr(new QFile(this))
{
	connect(this, &Console::finished,
			qApp, &QCoreApplication::quit,
			Qt::QueuedConnection);

	this->setTerminationEnabled(true);
	this->start();

	this->qOut->open(stdout, QIODevice::WriteOnly);
	this->qErr->open(stderr, QIODevice::WriteOnly);
}

Console::~Console()
{
	this->qOut->close();
	this->qErr->close();

	if(this->isRunning())
		this->terminate();
}

void Console::installAsMessageHandler()
{
	consoleInstance = this;
	qInstallMessageHandler(consoleMessage);
}

bool Console::autoExit() const
{
	return this->autoQuit;
}



void Console::writeMessage(bool isError, const QByteArray &message, bool lineFeed)
{
	QFile *file;
	if(isError)
		file = this->qErr;
	else
		file = this->qOut;

	file->write(message);
	if(lineFeed) {
		file->write("\n");
		file->flush();
	}
}

void Console::setAutoExit(bool autoExit)
{
	this->autoQuit = autoExit;
}

void Console::run()
{
	QFile inFile;
	inFile.open(stdin, QIODevice::ReadOnly);

	forever {
		auto command = inFile.readLine().trimmed();
		if(command.isEmpty())
			QThread::msleep(100);
		else if(this->autoQuit && command == QStringLiteral("exit"))
			break;
		else {
			QMetaObject::invokeMethod(this, "commandReceived", Qt::QueuedConnection,
									  Q_ARG(QByteArray, command));
		}
	}

	inFile.close();
}

void Console::consoleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(context);

	QString resStr;
	switch(type) {
	case QtMsgType::QtDebugMsg:
		resStr = QStringLiteral("[Debug]    ") + msg;
		break;
	case QtMsgType::QtWarningMsg:
		resStr = QStringLiteral("[Warning]  ") + msg;
		break;
	case QtMsgType::QtCriticalMsg:
		resStr = QStringLiteral("[Critical] ") + msg;
		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
		break;
	case QtMsgType::QtFatalMsg:
		resStr = QStringLiteral("[Fatal]    ") + msg;
		qt_assert_x(context.function, qUtf8Printable(resStr), context.file, context.line);
		break;
	case QtMsgType::QtInfoMsg:
		resStr = msg;
		break;
	default:
		Q_UNREACHABLE();
	}

	if(consoleInstance)
		consoleInstance->writeMessage(type != QtMsgType::QtInfoMsg, resStr.toUtf8());
	else
		std::cerr << "CONSOLE DESTROYED! ORIGINAL MESSAGE:" << qUtf8Printable(resStr);
}
