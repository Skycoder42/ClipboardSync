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

void InputReader::run()
{
	QFile inFile;
	inFile.open(stdin, QIODevice::ReadOnly);

	forever {
		auto command = inFile.readLine().trimmed();
		qDebug(command.constData());

		if(command == "exit")
			break;
	}
}
