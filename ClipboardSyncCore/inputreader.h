#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include "clipboardsynccore_global.h"

class CLIPBOARDSYNCCORESHARED_EXPORT InputReader : public QThread
{
	Q_OBJECT
public:
	explicit InputReader(QObject *parent = nullptr);

signals:

protected:
	void run() override;

private:
	QFile *qIn;
};

#endif // INPUTREADER_H
