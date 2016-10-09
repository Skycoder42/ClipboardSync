#include "inputreader.h"
#include <QTextStream>
#include <QCoreApplication>

InputReader::InputReader(QObject *parent) :
	QThread(parent),
	qIn(nullptr)
{
	connect(this, &InputReader::finished,
			qApp, &QCoreApplication::quit,
			Qt::QueuedConnection);

	this->setTerminationEnabled(true);
	this->start();
}

InputReader::~InputReader()
{
	if(this->isRunning())
		this->terminate();
}

bool InputReader::autoExit() const
{
	return this->autoQuit;
}

void InputReader::setAutoExit(bool autoExit)
{
	this->autoQuit = autoExit;
}

void InputReader::run()
{
	QFile inFile;
	inFile.open(stdin, QIODevice::ReadOnly);

	forever {
		auto command = inFile.readLine().trimmed();
		if(this->autoQuit && command == "exit")
			break;
		else
			emit commandReceived(command);
	}
}
