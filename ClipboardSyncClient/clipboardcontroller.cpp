#include "clipboardcontroller.h"
#include <QGuiApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMimeData>

#include <QDebug>

ClipboardController::ClipboardController(QObject *parent) :
	QObject(parent),
	clipboard(QGuiApplication::clipboard()),
	skipNext(false)
{
	connect(this->clipboard, &QClipboard::dataChanged,
			this, &ClipboardController::clipDataChanged);
}

void ClipboardController::setClipboard(const QByteArray &data)
{
	qDebug() << "Received clipboard data from server";
	auto doc = QJsonDocument::fromBinaryData(data);
	if(!doc.isNull()) {
		auto object = doc.object();
		if(object.isEmpty()) {
			this->skipNext = true;
			this->clipboard->clear();
		} else {
			auto mime = new QMimeData();
			foreach(auto key, object.keys())
				mime->setData(key, QByteArray::fromBase64(object[key].toString().toUtf8()));
			this->skipNext = true;
			this->clipboard->setMimeData(mime);
		}
	} else
		qWarning() << "Invalid data received!";
}

void ClipboardController::syncNow()
{
	this->clipDataChanged();
}

void ClipboardController::clipDataChanged()
{
	if(this->skipNext) {
		this->skipNext = false;
		return;
	}

	auto mime = this->clipboard->mimeData();
	auto formats = mime->formats();

	QJsonObject mimeObject;
	foreach(auto format, formats)
		mimeObject[format] = QString::fromUtf8(mime->data(format).toBase64());

	QJsonDocument doc(mimeObject);
	emit clipboardChanged(doc.toBinaryData());
}
