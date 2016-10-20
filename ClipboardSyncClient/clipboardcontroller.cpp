#include "clipboardcontroller.h"
#include <QGuiApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMimeData>
#include <QBuffer>
#include <QImage>
#include <QDebug>

ClipboardController::ClipboardController(QObject *parent) :
	QObject(parent),
	clipboard(QGuiApplication::clipboard()),
	skipNext(false),
	interval(0),
	syncTimer(new QTimer(this))
{
	connect(this->clipboard, &QClipboard::dataChanged,
			this, &ClipboardController::clipDataChanged);
	connect(this->syncTimer, &QTimer::timeout,
			this, &ClipboardController::syncTrigger);
}

int ClipboardController::syncInterval() const
{
	return this->interval;
}

void ClipboardController::setClipboard(const QByteArray &data)
{
	auto doc = QJsonDocument::fromBinaryData(data);
	if(!doc.isNull()) {
		auto object = doc.object();
		if(object.isEmpty()) {
			qDebug() << "Received clipboard clear from server";
			this->skipNext = true;
			this->clipboard->clear();
		} else {
			qDebug() << "Received clipboard data from server:" << object.keys();
			auto mime = new QMimeData();
			foreach(auto key, object.keys()) {
				auto data = QByteArray::fromBase64(object[key].toString().toUtf8());
				//handle images seperatly
				if(key == "application/x-qt-image") {
					QImage image;
					QBuffer buffer(&data);
					buffer.open(QIODevice::ReadOnly);
					image.load(&buffer, "png");
					buffer.close();
					mime->setImageData(image);
				} else
					mime->setData(key, data);
			}
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

void ClipboardController::clear()
{
	this->clipboard->clear();
}

void ClipboardController::setSyncInterval(int syncInterval)
{
	this->interval = syncInterval;
	if(syncInterval == 0)
		this->syncTimer->stop();
	else
		this->syncTimer->start(syncInterval);
}

void ClipboardController::clipDataChanged()
{
	if(this->skipNext)
		this->skipNext = false;
	else {
		this->oldState = this->getData();
		emit clipboardChanged(this->oldState);
	}
}

void ClipboardController::syncTrigger()
{
	auto nState = this->getData();
	if(nState != this->oldState) {
		this->oldState = nState;
		emit clipboardChanged(this->oldState);
	}
}

QByteArray ClipboardController::getData() const
{
	auto mime = this->clipboard->mimeData();
	auto formats = mime->formats();

	QJsonObject mimeObject;
	foreach(auto format, formats) {
		//handle images seperatly
		if(format == "application/x-qt-image") {
			auto image = mime->imageData().value<QImage>();
			QBuffer buffer;
			buffer.open(QIODevice::WriteOnly);
			image.save(&buffer, "png");
			buffer.close();
			mimeObject[format] = QString::fromUtf8(buffer.buffer().toBase64());
		} else
			mimeObject[format] = QString::fromUtf8(mime->data(format).toBase64());
	}

	return QJsonDocument(mimeObject).toBinaryData();
}
