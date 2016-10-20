#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QHash>
#include <QHostAddress>
#include <QProcess>
#include <QStandardItemModel>

class ToolManager : public QObject
{
	Q_OBJECT

public:
	enum Actions {
		//All
		Close = 0x01,
		Sync = 0x02,
		Clear = 0x03,
		Log = 0x04,

		//Server only
		Status = 0x11,
		Peers = 0x12
	};
	Q_ENUM(Actions)

	static const QString CertAny;
	static const QString CertSystem;

	struct ServerInfo {
		quint16 port;
		QStringList localAddresses;
		QString remoteAddress;

		ServerInfo();
	};

	explicit ToolManager(QObject *parent = nullptr);
	~ToolManager();

	bool hasName(const QString &name) const;
	QString createTitle(const QString &name, const QString &title) const;

public slots:
	void createServer(const QString &name,
					  const int port,
					  const QString &authPass,
					  const QString &certPath,
					  const QString &certPass,
					  bool localOnly);
	void createClient(const QString &name,
					  const QString address,
					  const QString &authPass,
					  const QString &certPath,
					  const QString &certFormat);

	void performAction(const QString &name, Actions action);
	void removeClient(const QString &serverName, const QString &clientName);

signals:
	void serverCreated(const QString &name);
	void clientCreated(const QString &name);
	void instanceClosed(const QString &name);
	void serverStatusLoaded(const QString &name, ServerInfo serverInfo);

	void showMessage(QtMsgType type, const QString &title, const QString &message);
	void showLog(const QString &name, const QByteArray &log);
	void showPeers(const QString &name, QStandardItemModel *peerModel);

private slots:
	void procStarted();
	void procErrorOccurred(QProcess::ProcessError error);
	void procFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void procOutReady();
	void procErrReady();

private:
	struct InstanceInfo {
		bool isServer;
		QByteArray outBuffer;
		QByteArray errBuffer;
		QByteArray errorLog;

		struct ServerAwaiter : public ServerInfo {
			bool doesAwait;

			bool isComplete() const;

			ServerAwaiter();
		} serverAwaiter;

		InstanceInfo(bool isServer = false);
	};

	QHash<QString, QProcess*> processes;
	QHash<QProcess*, InstanceInfo> procInfos;

	void doCreate(const QString &name, bool isServer, const QStringList &arguments);

	QString generateTitle(QProcess *process, const QString &title) const;
	QString procName(QProcess *process) const;
	bool isActive(QProcess *process) const;
	void remove(QProcess *process);

	QHash<QString, QProcess*>::ConstIterator findIter(QProcess *process) const;
};

#endif // TOOLMANAGER_H
