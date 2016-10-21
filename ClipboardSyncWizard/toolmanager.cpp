#include "toolmanager.h"
#include <dialogmaster.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>

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

const QString ToolManager::CertAny(QStringLiteral("any"));
const QString ToolManager::CertSystem(QStringLiteral("default"));

ToolManager::ToolManager(QObject *parent) :
	QObject(parent),
	processes(),
	procInfos(),
	svrBoot(),
	cltBoot(),
	isImporting(false)
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

bool ToolManager::initStartup()
{
	QDir appData(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	appData.mkpath(QStringLiteral("."));
	QFile autoConfig(appData.absoluteFilePath(QStringLiteral("startup.json")));
	if(autoConfig.open(QIODevice::ReadOnly)) {
		QJsonParseError error;
		auto doc = QJsonDocument::fromJson(autoConfig.readAll(), &error);
		autoConfig.close();
		if(error.error == QJsonParseError::NoError) {
			auto obj = doc.object();
			auto servers = obj[QStringLiteral("servers")].toArray();
			auto clients = obj[QStringLiteral("clients")].toArray();
			if(!servers.isEmpty() || !clients.isEmpty()) {
				emit allowCreate(false);
				this->svrBoot = servers;
				this->cltBoot = clients;
				this->initNext();
				return true;
			}
		} else
			emit showMessage(QtMsgType::QtWarningMsg, tr("Startup file corrupted!"), tr("The startup file is corrupted! Unable to start servers and clients automatically!"));
	}

	return false;
}

void ToolManager::createServer(const QString &name, const int port, const QString &authPass, const QString &certPath, const QString &certPass, bool localOnly, bool isAutoStart)
{
	QStringList args;
	QJsonObject config;
	config[QStringLiteral("type")] = QStringLiteral("server");

	config[QStringLiteral("port")] = port;
	if(port != 0)
		args.append({QStringLiteral("--port"), QString::number(port)});

	if(!authPass.isEmpty()) {
		args.append({QStringLiteral("--authentication"), authPass});
		config[QStringLiteral("authentication")] = authPass;
	} else
		config[QStringLiteral("authentication")] = QJsonValue::Null;

	if(!certPath.isEmpty()) {
		args.append({QStringLiteral("--secure"), certPath, QStringLiteral("--keypassphrase"), certPass});
		QJsonObject sec;
		sec[QStringLiteral("path")] = certPath;
		sec[QStringLiteral("passphrase")] = certPass;
		config[QStringLiteral("secure")] = sec;
	} else
		config[QStringLiteral("secure")] = QJsonValue::Null;

	if(localOnly)
		args.append(QStringLiteral("--localonly"));
	config[QStringLiteral("local")] = localOnly;

	args.append(name);
	config[QStringLiteral("name")] = name;

	this->doCreate(name, true, args, config, isAutoStart);
}

void ToolManager::createClient(const QString &name, const QString address, const QString &authPass, const QString &certPath, const QString &certFormat, bool isAutoStart)
{
	QStringList args;
	QJsonObject config;
	config[QStringLiteral("type")] = QStringLiteral("client");

	if(!authPass.isEmpty()) {
		args.append({QStringLiteral("--authentication"), authPass});
		config[QStringLiteral("authentication")] = authPass;
	} else
		config[QStringLiteral("authentication")] = QJsonValue::Null;

	if(!certPath.isEmpty()) {
		args.append({QStringLiteral("--secure"), certPath, QStringLiteral("--format"), certFormat});
		QJsonObject sec;
		sec[QStringLiteral("path")] = certPath;
		sec[QStringLiteral("format")] = certFormat;
		config[QStringLiteral("secure")] = sec;
	} else
		config[QStringLiteral("secure")] = QJsonValue::Null;

	args.append(name);
	args.append(address);	
	config[QStringLiteral("name")] = name;
	config[QStringLiteral("address")] = address;

	this->doCreate(name, false, args, config, isAutoStart);
}

void ToolManager::importInstance()
{
	auto file = DialogMaster::getOpenFileName(nullptr,
											  tr("Import Configuration"),
											  QString(),
											  tr("Clipboard-Sync Configuration Files (*.cscfg);;All Files (*)"));
	if(!file.isNull()) {
		QFile inFile(file);
		if(inFile.open(QIODevice::ReadOnly)) {
			QJsonParseError error;
			auto doc = QJsonDocument::fromJson(inFile.readAll(), &error);
			inFile.close();
			if(error.error == QJsonParseError::NoError) {
				auto obj = doc.object();
				this->isImporting = true;
				this->createFromConfig(obj, false);
				return;
			} else
				DialogMaster::warning(nullptr, tr("Failed to load configuration to file!"));
		}
	}

	emit importDone();
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
		case Save:
			this->doSave(name);
			break;
		case ToggleAutoSave:
			this->procInfos[proc].autoStart = !this->procInfos[proc].autoStart;
			this->rewriteAutoSave();
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

void ToolManager::removeClient(const QString &serverName, const QString &clientName)
{
	auto proc = this->processes.value(serverName, nullptr);
	if(proc && this->procInfos[proc].isServer)
		proc->write("close " + clientName.toUtf8() + "\n");
}

void ToolManager::setSyncInterval(const QString &clientName, int interval)
{
	auto proc = this->processes.value(clientName, nullptr);
	if(proc && !this->procInfos[proc].isServer)
		proc->write("interval " + QByteArray::number(interval) + "\n");
}

void ToolManager::procStarted()
{
	auto proc = qobject_cast<QProcess*>(QObject::sender());
	if(proc) {
		const auto &info = this->procInfos[proc];
		if(info.isServer) {
			proc->write("showconnectinfo\nport\nnetinfo\nremoteinfo\n");
			emit serverCreated(this->procName(proc), info.autoStart);
		} else
			emit clientCreated(this->procName(proc), info.autoStart);

		if(info.autoStart)
			this->initNext();
		if(this->isImporting) {
			this->isImporting = false;
			emit importDone();
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

	if(this->isImporting) {
		this->isImporting = false;
		emit importDone();
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

	if(this->isImporting) {
		this->isImporting = false;
		emit importDone();
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

				//check peers
				if(command == "peers") {
					QJsonParseError error;
					QJsonArray peerList = QJsonDocument::fromJson(param, &error).array();
					if(error.error == QJsonParseError::NoError) {
						auto model = new QStandardItemModel(0, 3, this);
						model->setHorizontalHeaderLabels({tr("Name"), tr("IP-Address"), tr("Port")});
						foreach(auto peer, peerList) {
							auto obj = peer.toObject();
							model->appendRow({
												 new QStandardItem(obj[QStringLiteral("name")].toString()),
												 new QStandardItem(obj[QStringLiteral("address")].toString()),
												 new QStandardItem(QString::number(obj[QStringLiteral("port")].toInt()))
											 });
						}
						emit showPeers(this->procName(proc), model);
					} else
						qWarning() << "Unable to parse result of \"peers\" command";
				}

				//status data
				if(this->procInfos[proc].serverAwaiter.doesAwait) {
					auto &awaiter = this->procInfos[proc].serverAwaiter;

					if(command == "port") {
						awaiter.port = param.toUShort();
						this->procInfos[proc].config[QStringLiteral("port")] = awaiter.port;
					} else if(command == "netinfo")
						awaiter.localAddresses = QString::fromUtf8(param).split(QLatin1Char(';'), QString::SkipEmptyParts);
					else if(command == "remoteinfo")
						awaiter.remoteAddress = QString::fromUtf8(param);

					if(awaiter.isComplete()) {
						emit serverStatusLoaded(this->procName(proc), awaiter);
						awaiter.doesAwait = false;
					}
				}
			} else {
				if(command == "servername")
					emit clientStatusLoaded(this->procName(proc), QString::fromUtf8(param));
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

void ToolManager::initNext()
{
	if(!this->svrBoot.isEmpty()) {
		auto server = this->svrBoot.takeAt(0).toObject();
		this->createFromConfig(server, true);
	} else if(!this->cltBoot.isEmpty()) {
		auto client = this->cltBoot.takeAt(0).toObject();
		this->createFromConfig(client, true);
	} else
		emit allowCreate(true);
}

void ToolManager::rewriteAutoSave()
{
	QJsonArray servers;
	QJsonArray clients;

	foreach(auto procInfo, this->procInfos) {
		if(procInfo.autoStart) {
			if(procInfo.isServer)
				servers.append(procInfo.config);
			else
				clients.append(procInfo.config);
		}
	}

	QJsonObject obj;
	obj[QStringLiteral("servers")] = servers;
	obj[QStringLiteral("clients")] = clients;

	QDir appData(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	appData.mkpath(QStringLiteral("."));
	QFile autoConfig(appData.absoluteFilePath(QStringLiteral("startup.json")));
	if(autoConfig.open(QIODevice::WriteOnly)) {
		QJsonDocument doc(obj);
		autoConfig.write(doc.toJson(QJsonDocument::Indented));
		autoConfig.close();
	}
}

void ToolManager::createFromConfig(const QJsonObject &config, bool isAutoStart)
{
	if(config[QStringLiteral("type")] == QStringLiteral("server")) {
		auto auth = config[QStringLiteral("authentication")];
		auto sec = config[QStringLiteral("secure")];

		this->createServer(config[QStringLiteral("name")].toString(),
						   config[QStringLiteral("port")].toInt(),
						   auth.isNull() ? QString() : auth.toString(),
						   sec.isNull() ? QString() : sec.toObject()[QStringLiteral("path")].toString(),
						   sec.isNull() ? QString() : sec.toObject()[QStringLiteral("passphrase")].toString(),
						   config[QStringLiteral("local")].toBool(),
						   isAutoStart);
	} else if(config[QStringLiteral("type")] == QStringLiteral("client")) {
		auto auth = config[QStringLiteral("authentication")];
		auto sec = config[QStringLiteral("secure")];

		this->createClient(config[QStringLiteral("name")].toString(),
						   config[QStringLiteral("address")].toString(),
						   auth.isNull() ? QString() : auth.toString(),
						   sec.isNull() ? QString() : sec.toObject()[QStringLiteral("path")].toString(),
						   sec.isNull() ? QString() : sec.toObject()[QStringLiteral("format")].toString(),
						   isAutoStart);
	}
}

void ToolManager::doCreate(const QString &name, bool isServer, const QStringList &arguments, const QJsonObject &config, bool isAutoStart)
{
	auto proc = new QProcess(this);
	proc->setProgram(isServer ? SERVER_TOOL : CLIENT_TOOL);
	proc->setArguments(arguments);

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
	this->procInfos.insert(proc, {isServer, config, isAutoStart});
	proc->start(QIODevice::ReadWrite);
}

void ToolManager::doSave(const QString &name)
{
	auto proc = this->processes.value(name, nullptr);
	if(proc) {
		auto info = this->procInfos[proc];
		auto file = DialogMaster::getSaveFileName(nullptr,
												  info.isServer ?
													  tr("Export Server Configuration") :
													  tr("Export Client Configuration"),
												  QString(),
												  tr("Clipboard-Sync Configuration Files (*.cscfg);;All Files (*)"));
		if(!file.isNull()) {
			QFile outFile(file);
			if(outFile.open(QIODevice::WriteOnly)) {
				QJsonDocument doc(info.config);
				outFile.write(doc.toJson(QJsonDocument::Indented));
				outFile.close();
			} else
				DialogMaster::warning(nullptr, tr("Failed to save configuration to file!"));
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
	this->rewriteAutoSave();
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

ToolManager::InstanceInfo::InstanceInfo(bool isServer, const QJsonObject &config, bool isAutoStart) :
	isServer(isServer),
	config(config),
	autoStart(isAutoStart),
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
