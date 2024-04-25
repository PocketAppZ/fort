#include "autoupdatemanagerrpc.h"

#include <control/controlworker.h>
#include <rpc/rpcmanager.h>
#include <util/ioc/ioccontainer.h>
#include <util/osutil.h>

namespace {

inline bool processAutoUpdateManager_updateState(
        AutoUpdateManager *autoUpdateManager, const ProcessCommandArgs &p)
{
    if (auto aum = qobject_cast<AutoUpdateManagerRpc *>(autoUpdateManager)) {
        aum->updateState(
                p.args.value(0).toBool(), p.args.value(1).toBool(), p.args.value(2).toInt());
    }
    return true;
}

inline bool processAutoUpdateManager_restartClients(
        AutoUpdateManager *autoUpdateManager, const ProcessCommandArgs &p)
{
    const QString installerPath = p.args.value(0).toString();

    if (qobject_cast<AutoUpdateManagerRpc *>(autoUpdateManager)) {
        OsUtil::restartClient(installerPath);
    } else {
        emit autoUpdateManager->restartClients(installerPath);
    }
    return true;
}

bool processAutoUpdateManager_startDownload(AutoUpdateManager *autoUpdateManager,
        const ProcessCommandArgs & /*p*/, QVariantList & /*resArgs*/)
{
    return autoUpdateManager->startDownload();
}

using processAutoUpdateManager_func = bool (*)(
        AutoUpdateManager *autoUpdateManager, const ProcessCommandArgs &p, QVariantList &resArgs);

static processAutoUpdateManager_func processAutoUpdateManager_funcList[] = {
    &processAutoUpdateManager_startDownload, // Rpc_AutoUpdateManager_startDownload,
};

inline bool processAutoUpdateManagerRpcResult(
        AutoUpdateManager *autoUpdateManager, const ProcessCommandArgs &p, QVariantList &resArgs)
{
    const processAutoUpdateManager_func func = RpcManager::getProcessFunc(p.command,
            processAutoUpdateManager_funcList, Control::Rpc_AutoUpdateManager_startDownload,
            Control::Rpc_AutoUpdateManager_startDownload);

    return func ? func(autoUpdateManager, p, resArgs) : false;
}

}

AutoUpdateManagerRpc::AutoUpdateManagerRpc(const QString &cachePath, QObject *parent) :
    AutoUpdateManager(cachePath, parent)
{
}

void AutoUpdateManagerRpc::setBytesReceived(int v)
{
    if (m_bytesReceived != v) {
        m_bytesReceived = v;
        emit bytesReceivedChanged(v);
    }
}

void AutoUpdateManagerRpc::setUp()
{
    AutoUpdateManager::setUp();

    setupClientSignals();
}

void AutoUpdateManagerRpc::updateState(bool isDownloaded, bool isDownloading, int bytesReceived)
{
    setIsDownloaded(isDownloaded);
    setIsDownloading(isDownloading);
    setBytesReceived(bytesReceived);
}

QVariantList AutoUpdateManagerRpc::updateState_args()
{
    auto autoUpdateManager = IoC<AutoUpdateManager>();

    const bool isDownloaded = autoUpdateManager->isDownloaded();
    const bool isDownloading = autoUpdateManager->isDownloading();
    const int bytesReceived =
            isDownloaded ? autoUpdateManager->downloadSize() : autoUpdateManager->bytesReceived();

    return { isDownloaded, isDownloading, bytesReceived };
}

bool AutoUpdateManagerRpc::processInitClient(ControlWorker *w)
{
    return w->sendCommand(Control::Rpc_AutoUpdateManager_updateState, updateState_args());
}

bool AutoUpdateManagerRpc::processServerCommand(
        const ProcessCommandArgs &p, QVariantList &resArgs, bool &ok, bool &isSendResult)
{
    auto autoUpdateManager = IoC<AutoUpdateManager>();

    switch (p.command) {
    case Control::Rpc_AutoUpdateManager_updateState: {
        return processAutoUpdateManager_updateState(autoUpdateManager, p);
    }
    case Control::Rpc_AutoUpdateManager_restartClients: {
        return processAutoUpdateManager_restartClients(autoUpdateManager, p);
    }
    default: {
        ok = processAutoUpdateManagerRpcResult(autoUpdateManager, p, resArgs);
        isSendResult = true;
        return true;
    }
    }
}

void AutoUpdateManagerRpc::setupServerSignals(RpcManager *rpcManager)
{
    auto autoUpdateManager = IoC<AutoUpdateManager>();

    const auto updateClientStates = [=] {
        rpcManager->invokeOnClients(Control::Rpc_AutoUpdateManager_updateState,
                AutoUpdateManagerRpc::updateState_args());
    };

    connect(autoUpdateManager, &AutoUpdateManager::isDownloadingChanged, rpcManager,
            updateClientStates);
    connect(autoUpdateManager, &AutoUpdateManager::bytesReceivedChanged, rpcManager,
            updateClientStates);

    connect(autoUpdateManager, &AutoUpdateManager::restartClients, rpcManager,
            [=](const QString &installerPath) {
                rpcManager->invokeOnClients(
                        Control::Rpc_AutoUpdateManager_restartClients, { installerPath });
            });
}

bool AutoUpdateManagerRpc::startDownload()
{
    return IoC<RpcManager>()->doOnServer(Control::Rpc_AutoUpdateManager_startDownload);
}

void AutoUpdateManagerRpc::setupClientSignals()
{
    auto rpcManager = IoCDependency<RpcManager>();

    connect(this, &AutoUpdateManager::restartClients, rpcManager,
            [=](const QString &installerPath) {
                rpcManager->invokeOnServer(
                        Control::Rpc_AutoUpdateManager_restartClients, { installerPath });
            });
}