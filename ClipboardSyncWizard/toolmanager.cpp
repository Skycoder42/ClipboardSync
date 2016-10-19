#include "toolmanager.h"
#include <dialogmaster.h>

#ifdef QT_NO_DEBUG
# define SERVER_TOOL QStringLiteral("./ClipboardSyncServer")
# define CLIENT_TOOL QStringLiteral("./ClipboardSyncClient")
#else
# include <QDir>
# ifdef Q_OS_WIN
#  define SERVER_TOOL QStringLiteral("../ClipboardSyncServer/debug/ClipboardSyncServer")
#  define CLIENT_TOOL QStringLiteral("../ClipboardSyncClient/debug/ClipboardSyncClient")
# endif
#endif

ToolManager::ToolManager(QObject *parent) :
	QObject(parent),
	processes(),
	procInfos()
{}

ToolManager::~ToolManager()
{
	foreach (auto proc, this->processes)
		proc->write("exit\n");

	auto allowWait = true;
	foreach (auto proc, this->processes) {
		if(!allowWait || !proc->waitForFinished(1000)) {
			allowWait = false;
			proc->kill();
		}
	}
}

bool ToolManager::hasName(const QString &name) const
{
	return this->processes.contains(name);
}

QString ToolManager::createTitle(const QString &name, const QString &title) const
{
	auto proc = this->processes.value(name, nullptr);
	if(proc)
		return this->generateTitle(proc, title);
	else
		return title;
}

void ToolManager::createServer(const QString &name, const int port, const QString &authPass, const QString &certPath, const QString &certPass, bool localOnly)
{
	auto proc = new QProcess(this);
	proc->setProgram(SERVER_TOOL);

	QStringList args;
	if(port != 0)
		args.append({QStringLiteral("--port"), QString::number(port)});
	if(!authPass.isEmpty())
		args.append({QStringLiteral("--authentication"), authPass});
	if(!certPath.isEmpty())
		args.append({QStringLiteral("--secure"), certPath, QStringLiteral("--keypassphrase"), certPass});
	if(localOnly)
		args.append(QStringLiteral("--localonly"));
	args.append(name);
	proc->setArguments(args);

	connect(proc, &QProcess::started,
			this, &ToolManager::procStarted);
	connect(proc, &QProcess::errorOccurred,
			this, &ToolManager::procErrorOccurred);
	connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			this, &ToolManager::procFinished);
	connect(proc, &QProcess::readyReadStandardOutput,
			this, &ToolManager::procOutReady);
	connect(proc, &QProcess::readyReadStandardError,
			this, &ToolManager::procErrReady);

#if !defined(QT_NO_DEBUG) && defined(Q_OS_WIN)
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	auto path = env.value(QStringLiteral("PATH"));
	path.append(QLatin1Char(';') + QDir(QStringLiteral("../ClipboardSyncCore/debug/")).absolutePath());
	env.insert(QStringLiteral("PATH"), path);
	proc->setEnvironment(env.toStringList());
#endif

	this->processes.insert(name, proc);
	this->procInfos.insert(proc, true);
	proc->start(QIODevice::ReadWrite);
}

void ToolManager::performAction(const QString &name, ToolManager::Actions action)
{
	auto proc = this->processes.value(name, nullptr);
	if(proc) {
		switch(action) {
		case Close:
			proc->write("exit\n");
			break;
		case Sync:
			proc->write("sync\n");
			break;
		case Clear:
			proc->write("clear\n");
			break;
		case Log:
			emit showLog(name, this->procInfos[proc].errorLog);
			break;
		case Status:
			this->procInfos[proc].serverAwaiter.doesAwait = true;
			proc->write("port\nnetinfo\nremoteinfo\n");
			break;
		case Peers:
			proc->write("peers\n");
			break;
		default:
			Q_UNREACHABLE();
		}
	}
}

void ToolManager::procStarted()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {//TODO
		if(this->procInfos[proc].isServer) {
			proc->write("showconnectinfo\nport\nnetinfo\nremoteinfo\n");
			emit serverCreated(this->procName(proc));
		}
	}
}

void ToolManager::procErrorOccurred(QProcess::ProcessError error)
{
	Q_UNUSED(error);
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		emit showMessage(QtMsgType::QtCriticalMsg, this->generateTitle(proc, tr("Error occured!")), proc->errorString());
		this->remove(proc);
	}
}

void ToolManager::procFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		if(exitStatus != QProcess::NormalExit)
			emit showMessage(QtMsgType::QtCriticalMsg, this->generateTitle(proc, tr("Process Crashed!")), proc->errorString());
		else
			emit showMessage(QtMsgType::QtInfoMsg, this->generateTitle(proc, tr("Finished!")), tr("The Process was closed!"));
		emit instanceClosed(this->procName(proc));
		this->remove(proc);
	}
}

void ToolManager::procOutReady()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {
		auto out = this->procInfos[proc].outBuffer + proc->readAllStandardOutput();
		auto args = out.split('\n');
		this->procInfos[proc].outBuffer = args.takeLast();

		foreach(auto info, args) {
			auto sIndex = info.indexOf(':');
			auto command = info.mid(0, sIndex).toLower();
			auto param = info.mid(sIndex + 1).simplified();

			if(this->procInfos[proc].isServer) {
				//check "info"
				if(command == "info") {
					auto str = QString::fromUtf8(param);
					auto bIndex = str.indexOf(QLatin1Char(';'));
					auto title = str.mid(0, bIndex);
					auto text = str.mid(bIndex + 1);
					emit showMessage(QtMsgType::QtInfoMsg, title, text);
				}

				//init data
				if(this->procInfos[proc].serverAwaiter.doesAwait) {
					auto &awaiter = this->procInfos[proc].serverAwaiter;

					if(command == "port")
						awaiter.port = param.toUShort();
					else if(command == "netinfo")
						awaiter.localAddresses = QString::fromUtf8(param).split(QLatin1Char(';'), QString::SkipEmptyParts);
					else if(command == "remoteinfo")
						awaiter.remoteAddress = QString::fromUtf8(param);

					if(awaiter.isComplete()) {
						emit serverStatusLoaded(this->procName(proc), awaiter);
						awaiter.doesAwait = false;
					}
				}
			} else {

			}
		}
	}
}

void ToolManager::procErrReady()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {
		auto allErr = proc->readAllStandardError();
		this->procInfos[proc].errorLog.append(allErr);

		auto err = this->procInfos[proc].errBuffer + allErr;
		auto args = err.split('\n');
		this->procInfos[proc].errBuffer = args.takeLast();

		foreach(auto info, args) {
			auto type = info.mid(0, 11).simplified();
			auto message = info.mid(11);

			QtMsgType mType;
			QString title;
			if(type == "[Warning]") {
				mType = QtMsgType::QtWarningMsg;
				title = this->generateTitle(proc, tr("Warning!"));
			} else if(type == "[Critical]") {
				mType = QtMsgType::QtCriticalMsg;
				title = this->generateTitle(proc, tr("Critical Error!!!"));
			} else if(type == "[Critical]") {
				mType = QtMsgType::QtFatalMsg;
				title = this->generateTitle(proc, tr("⚠ Fatal Error ⚠"));
			} else
				continue;

			emit showMessage(mType, title, message);
		}
	}
}

QString ToolManager::generateTitle(QProcess *process, const QString &title) const
{
	return tr("%1 %2 — %3")
			.arg(this->procInfos[process].isServer ? tr("Server") : tr("Client"))
			.arg(this->procName(process))
			.arg(title);
}

QString ToolManager::procName(QProcess *process) const
{
	return this->findIter(process).key();
}

bool ToolManager::isActive(QProcess *process) const
{
	return this->findIter(process) != this->processes.constEnd();
}

void ToolManager::remove(QProcess *process)
{
	this->processes.erase(this->findIter(process));
	this->procInfos.remove(process);
	process->deleteLater();
}

QHash<QString, QProcess*>::ConstIterator ToolManager::findIter(QProcess *process) const
{
	return std::find_if(this->processes.constBegin(), this->processes.constEnd(), [=](auto value){
		return value == process;
	});
}



ToolManager::ServerInfo::ServerInfo() :
	port(0),
	localAddresses(),
	remoteAddress()
{}

ToolManager::InstanceInfo::InstanceInfo(bool isServer) :
	isServer(isServer),
	outBuffer(),
	errBuffer(),
	errorLog(),
	serverAwaiter()
{}

bool ToolManager::InstanceInfo::ServerAwaiter::isComplete() const
{
	return this->port != 0 &&
			!this->localAddresses.isEmpty() &&
			!this->remoteAddress.isNull();
}

ToolManager::InstanceInfo::ServerAwaiter::ServerAwaiter() :
	ServerInfo(),
	doesAwait(true)
{}
