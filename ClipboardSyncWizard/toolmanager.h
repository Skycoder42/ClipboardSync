#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QHash>
#include <QHostAddress>
#include <QProcess>

class ToolManager : public QObject
{
	Q_OBJECT
public:
	explicit ToolManager(QObject *parent = nullptr);
	~ToolManager();

	bool hasName(const QString &name) const;

public slots:
	void createServer(const QString &name,
					  const int port,
					  const QString &authPass,
					  const QString &certPath,
					  const QString &cerPass,
					  bool localOnly);

signals:
	void errorOccured(bool isServer, const QString &name, const QString &error);
	void serverCreated(const QString &name, quint16 port, const QStringList &localAddresses, const QString &remoteAddress);

private slots:
	void procStarted();
	void procErrorOccurred(QProcess::ProcessError error);
	void procFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void procOutReady();

private:
	typedef QPair<QProcess*, bool> ProcInfo;
	struct ServerInfo {
		int port;
		QStringList localAddresses;
		QString remoteAddress;
	};

	QHash<QString, ProcInfo> processes;

	QHash<QProcess*, QByteArray> outBuffer;
	QHash<QProcess*, ServerInfo> portAwaiters;

	QString procName(QProcess *proccess) const;
	bool isServer(QProcess *proccess) const;
	bool isActive(QProcess *proccess) const;
	void remove(QProcess *proccess);

	QHash<QString, ProcInfo>::ConstIterator findIter(QProcess *proccess) const;
};

#endif // TOOLMANAGER_H
