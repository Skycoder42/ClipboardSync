#ifndef CLIPBOARDCONTROLLER_H
#define CLIPBOARDCONTROLLER_H

#include <QObject>
#include <QClipboard>
#include <QTimer>

class ClipboardController : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int syncInterval READ syncInterval WRITE setSyncInterval)

public:
	explicit ClipboardController(QObject *parent = nullptr);

	int syncInterval() const;

public slots:
	void setClipboard(const QByteArray &data);
	void syncNow();
	void clear();

	void setSyncInterval(int syncInterval);

signals:
	void clipboardChanged(const QByteArray &data);

private slots:
	void clipDataChanged();
	void syncTrigger();

private:
	QClipboard *clipboard;
	bool skipNext;
	int interval;

	QTimer *syncTimer;
	QByteArray oldState;

	QByteArray getData() const;
};

#endif // CLIPBOARDCONTROLLER_H
