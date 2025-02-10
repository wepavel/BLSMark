#include "httpmanager.h"
#include "globalsettings.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

HttpManager::HttpManager(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

void HttpManager::makeRequest(const QUrl &url,
                              const QJsonDocument &jsonDoc,
                              const HttpMethod &method,
                              const std::function<void (const QByteArray &)> &successCallback)
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
        break;
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec(); // Блокировка пока не придет ответа

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        successCallback(responseData);
    } else {
        emit requestError(reply->errorString());
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
