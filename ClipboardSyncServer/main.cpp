#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <iostream>
#include "inputreader.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	InputReader reader;

	return a.exec();
}
