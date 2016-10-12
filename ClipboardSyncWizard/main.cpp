#include "mainwizard.h"
#include <QApplication>
#include <QIcon>
#include <QSsl>

Q_DECLARE_METATYPE(QSsl::EncodingFormat)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(TARGET);
	QApplication::setApplicationVersion(VERSION);
	QApplication::setOrganizationName(COMPANY);
	QApplication::setOrganizationDomain(QStringLiteral("com.Skycoder42"));
	QApplication::setApplicationDisplayName(DISPLAY_NAME);
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));

	qRegisterMetaType<QSsl::EncodingFormat>();

	MainWizard w;
	w.show();

	return a.exec();
}
