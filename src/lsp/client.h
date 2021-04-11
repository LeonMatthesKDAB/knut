#pragma once

#include "initialize_request.h"
#include "shutdown_request.h"

#include <QObject>

#include <spdlog/spdlog.h>

#include <functional>
#include <unordered_map>

class QProcess;

namespace Lsp {

class Client : public QObject
{
    Q_OBJECT

public:
    Client(const std::string &language, const QString &program, const QStringList &arguments,
           QObject *parent = nullptr);

    void start();
    void shutdown();

    template <typename Request>
    void sendRequest(Request request, typename Request::ResponseCallback callback)
    {
        m_callbacks[request.id] = [this, callback](nlohmann::json j) {
            if (callback) {
                auto response = j.get<typename Request::Response>();
                if (!response.isValid())
                    m_serverLogger->error("==> Invalid response from server: {}", j.dump());
                callback(response);
            }
        };
        sendJsonRequest(request);
    }

    template <typename Notification>
    void sendNotification(Notification notification)
    {
        sendJsonNotification(notification);
    }

signals:
    void initialized();
    void finished();
    void errorOccured(int error);

private:
    void readError();
    void readOutput();
    void initialize();
    void exitServer();
    void handleError(int error);

    void sendJsonRequest(nlohmann::json jsonRequest);
    void sendJsonNotification(nlohmann::json jsonNotification);

    void initializeCallback(InitializeRequest::Response response);
    void shutdownCallback(ShutdownRequest::Response response);

    void logMessage(std::string type, const nlohmann::json &message);

private:
    std::shared_ptr<spdlog::logger> m_serverLogger;
    std::shared_ptr<spdlog::logger> m_messageLogger;
    const QString m_program;
    const QStringList m_arguments;
    QProcess *m_process = nullptr;

    std::unordered_map<MessageId, std::function<void(nlohmann::json)>> m_callbacks;
};

}