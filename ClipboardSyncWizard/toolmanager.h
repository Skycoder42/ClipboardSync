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
	enum Actions {
		//All
		Close = 0x01,
		Sync = 0x02,
		Clear = 0x03,

		//Server only
		Status = 0x11,
		Peers = 0x12
	};
	Q_ENUM(Actions)

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

	void performAction(const QString &name, Actions action);

signals:
	void showMessage(QtMsgType type, const QString &title, const QString &message);
	void serverCreated(const QString &name, quint16 port, const QStringList &localAddresses, const QString &remoteAddress);

	void instanceClosed(const QString &name);

private slots:
	void procStarted();
	void procErrorOccurred(QProcess::ProcessError error);
	void procFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void procOutReady();

private:
	struct InstanceInfo {
		bool isServer;
		QByteArray outBuffer;
		QByteArray errBuffer;

		struct ServerAwaiter {
			bool doesAwait;
			bool initAwait;

			int port;
			QStringList localAddresses;
			QString remoteAddress;

			bool isComplete() const;
		} serverAwaiter;

		InstanceInfo(bool isServer = false);
	};

	QHash<QString, QProcess*> processes;
	QHash<QProcess*, InstanceInfo> procInfos;

	QString generateTitle(QProcess *process, const QString &title);
	QString procName(QProcess *process) const;
	bool isActive(QProcess *process) const;
	void remove(QProcess *process);

	QHash<QString, QProcess*>::ConstIterator findIter(QProcess *process) const;
};

#endif // TOOLMANAGER_H
