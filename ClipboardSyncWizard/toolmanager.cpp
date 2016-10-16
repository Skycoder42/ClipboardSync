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
	servers(),
	clients()
{}

ToolManager::~ToolManager()
{
	auto endList = this->servers.values();
	endList.append(this->clients.values());

	foreach (auto proc, endList)
		proc->write("exit\n");

	auto allowWait = true;
	foreach (auto proc, endList) {
		if(!allowWait || !proc->waitForFinished(1000)) {
			allowWait = false;
			proc->kill();
		}
	}
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

	this->servers.insert(name, proc);
	this->portAwaiters.append(proc);
	proc->start(QIODevice::ReadWrite);
}

void ToolManager::procStarted()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {//TODO
	}
}

void ToolManager::procErrorOccurred(QProcess::ProcessError error)
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		this->remove(proc);
		emit errorOccured(this->isServer(proc), this->procName(proc), proc->errorString());
	}
}

void ToolManager::procFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc && this->isActive(proc)) {
		if(exitStatus != QProcess::NormalExit)
			this->procErrorOccurred(QProcess::Crashed);
		else
			emit errorOccured(this->isServer(proc), this->procName(proc), tr("Proccess was closed!"));
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
				if(command == "port" && this->portAwaiters.contains(proc)) {
					this->portAwaiters.removeOne(proc);
					emit serverCreated(this->procName(proc), param.toUShort(), {});
				}
			} else {

			}
		}
	}
}

QString ToolManager::procName(QProcess *proccess) const
{
	auto name = this->servers.key(proccess);
	if(name.isNull())
		return this->clients.key(proccess);
	else
		return name;
}

bool ToolManager::isServer(QProcess *proccess) const
{
	return !this->servers.key(proccess).isNull();
}

bool ToolManager::isActive(QProcess *proccess) const
{
	return !this->servers.key(proccess).isNull() ||
			!this->clients.key(proccess).isNull();
}

void ToolManager::remove(QProcess *proccess)
{
	this->servers.remove(this->servers.key(proccess));
	this->clients.remove(this->servers.key(proccess));
	this->portAwaiters.removeOne(proccess);
	proccess->deleteLater();
}
