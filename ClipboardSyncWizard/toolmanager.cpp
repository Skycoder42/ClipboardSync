#include "toolmanager.h"

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
	outBuffer(),
	portAwaiters()
{}

ToolManager::~ToolManager()
{
	foreach (auto proc, this->processes)
		proc.first->write("exit\n");

	auto allowWait = true;
	foreach (auto proc, this->processes) {
		if(!allowWait || !proc.first->waitForFinished(1000)) {
			allowWait = false;
			proc.first->kill();
		}
	}
}

bool ToolManager::hasName(const QString &name) const
{
	return this->processes.contains(name);
}

void ToolManager::createServer(const QString &name, const int port, const QString &authPass, const QString &certPath, const QString &cerPass, bool localOnly)
{
	auto proc = new QProcess(this);
	proc->setProgram(SERVER_TOOL);

	QStringList args;
	if(port != 0)
		args.append({QStringLiteral("--port"), QString::number(port)});
	if(!authPass.isEmpty())
		args.append({QStringLiteral("--authentication"), authPass});
	if(!certPath.isEmpty())
		args.append({QStringLiteral("--secure"), certPath, QStringLiteral("--keypassphrase"), cerPass});
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

#if !defined(QT_NO_DEBUG) && defined(Q_OS_WIN)
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	auto path = env.value(QStringLiteral("PATH"));
	path.append(QLatin1Char(';') + QDir(QStringLiteral("../ClipboardSyncCore/debug/")).absolutePath());
	env.insert(QStringLiteral("PATH"), path);
	proc->setEnvironment(env.toStringList());
#endif

	this->processes.insert(name, {proc, true});
	this->portAwaiters.insert(proc, {0, {}});
	proc->start(QIODevice::ReadWrite);
}

void ToolManager::procStarted()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {//TODO
		if(this->isServer(proc))
			proc->write("port\nnetinfo\nremoteinfo\n");
	}
}

void ToolManager::procErrorOccurred(QProcess::ProcessError error)
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		emit showMessage(QtMsgType::QtCriticalMsg, this->generateTitle(proc, tr("Error occured!")), proc->errorString());
		this->remove(proc);
	}
}

void ToolManager::procFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		if(exitStatus != QProcess::NormalExit)
			this->procErrorOccurred(QProcess::Crashed);
		else
			emit showMessage(QtMsgType::QtInfoMsg, this->generateTitle(proc, tr("Finished!")), tr("The Process was closed!"));
		this->remove(proc);
	}
}

void ToolManager::procOutReady()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {
		auto out = this->outBuffer.value(proc) + proc->readAllStandardOutput();
		auto args = out.split('\n');
		this->outBuffer.insert(proc, args.takeLast());

		foreach(auto info, args) {
			auto sIndex = info.indexOf(':');
			auto command = info.mid(0, sIndex).toLower();
			auto param = info.mid(sIndex + 1).simplified();

			if(this->isServer(proc)) {
				//init data
				if(this->portAwaiters.contains(proc)) {
					auto &status = this->portAwaiters[proc];

					if(command == "port")
						status.port = param.toUShort();
					else if(command == "netinfo")
						status.localAddresses = QString::fromUtf8(param).split(QLatin1Char(';'), QString::SkipEmptyParts);
					else if(command == "remoteinfo")
						status.remoteAddress = QString::fromUtf8(param);

					if(status.port != 0 && !status.localAddresses.isEmpty() && !status.remoteAddress.isNull()) {
						emit serverCreated(this->procName(proc), status.port, status.localAddresses, status.remoteAddress);
						this->portAwaiters.remove(proc);
					}
				}
			} else {

			}
		}
	}
}

QString ToolManager::generateTitle(QProcess *process, const QString &title)
{
	return tr("%1 %2 — %3")
			.arg(this->isServer(process) ? tr("Server") : tr("Client"))
			.arg(this->procName(process))
			.arg(title);
}

QString ToolManager::procName(QProcess *process) const
{
	return this->findIter(process).key();
}

bool ToolManager::isServer(QProcess *process) const
{
	return this->findIter(process)->second;
}

bool ToolManager::isActive(QProcess *process) const
{
	return this->findIter(process) != this->processes.constEnd();
}

void ToolManager::remove(QProcess *process)
{
	this->processes.erase(this->findIter(process));
	this->outBuffer.remove(process);
	this->portAwaiters.remove(process);
	process->deleteLater();
}

QHash<QString, ToolManager::ProcInfo>::ConstIterator ToolManager::findIter(QProcess *process) const
{
	return std::find_if(this->processes.constBegin(), this->processes.constEnd(), [=](auto value){
		return value.first == process;
	});
}
