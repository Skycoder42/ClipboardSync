#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include "clipboardsynccore_global.h"

class CLIPBOARDSYNCCORESHARED_EXPORT InputReader : public QThread
{
	Q_OBJECT

	Q_PROPERTY(bool autoExit READ autoExit WRITE setAutoExit)

public:
	explicit InputReader(QObject *parent = nullptr);
	~InputReader();

	bool autoExit() const;

public slots:
	void setAutoExit(bool autoExit);

signals:
	void commandReceived(const QByteArray &command);

protected:
	void run() override;

private:
	QFile *qIn;
	bool autoQuit;
};

#endif // INPUTREADER_H
