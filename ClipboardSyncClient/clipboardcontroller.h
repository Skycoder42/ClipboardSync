#ifndef CLIPBOARDCONTROLLER_H
#define CLIPBOARDCONTROLLER_H

#include <QObject>
#include <QClipboard>

class ClipboardController : public QObject
{
	Q_OBJECT
public:
	explicit ClipboardController(QObject *parent = nullptr);

public slots:
	void setClipboard(const QByteArray &data);
	void syncNow();

signals:
	void clipboardChanged(const QByteArray &data);

private slots:
	void clipDataChanged();

private:
	QClipboard *clipboard;
	bool skipNext;
};

#endif // CLIPBOARDCONTROLLER_H
