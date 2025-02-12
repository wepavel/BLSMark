#include "httpmanager.h"
#include "globalsettings.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

#include <QTimer>

HttpManager::HttpManager(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

void HttpManager::makeRequest(const QUrl &url,
                              const QJsonDocument &jsonDoc,
                              const HttpMethod &method,
                              const std::function<void (const QByteArray &, int)> &callback)
{
    QNetworkReply *reply = nullptr;
    QNetworkRequest request(url);

    switch (method) {
    case HttpMethod::Get:
    {
        reply = get(request);
        break;
    }
    case HttpMethod::Post:
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = post(request, jsonDoc.toJson());
        break;
    }
    default:
        emit requestError("Unknown HTTP method");
        callback(QByteArray(), -1);
        return;
    }

    QTimer timer;
    QEventLoop loop;

    // Устанавливаем таймер на 5 секунд (можно изменить по необходимости)
    timer.setSingleShot(true);
    timer.setInterval(500);

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    int statusCode = -1;

    if (timer.isActive()) {
        // Ответ получен вовремя
        timer.stop();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            callback(responseData, statusCode);
        } else {
            emit requestError(reply->errorString());
            statusCode = -1;
            callback(QByteArray(), statusCode);
        }
    } else {
        // Таймаут - нет подключения
        disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        reply->abort();
        emit requestError("Connection timeout");
        statusCode = -1;
        callback(QByteArray(), statusCode);
    }

    reply->deleteLater();
}

QUrl HttpManager::createApiUrl(const QString &endpoint)
{
    return QUrl(QString("http://%1:%2/api/v1/code-processing/%3")
                    .arg(gSettings.getBackendServiceIP())
                    .arg(gSettings.getBackendServicePort())
                    .arg(endpoint));
}
