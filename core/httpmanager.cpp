#include "httpmanager.h"
#include "globalsettings.h"
#include "qjsondocument.h"
#include "qnetworkreply.h"

#include <QTimer>
#include <QJsonObject>

#define HTTP_REQUEST_TIMEOUT 3000

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
        reply = get(request);
        break;
    case HttpMethod::Post:
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = post(request, jsonDoc.toJson());
        break;
    default:
        emit requestError("Unknown HTTP method");
        callback(QByteArray(), -1);
        return;
    }

    QTimer timer;
    QEventLoop loop;

    timer.setSingleShot(true);
    timer.setInterval(HTTP_REQUEST_TIMEOUT);

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        if (reply->isRunning()) {
            reply->abort();
            emit requestError("Connection timeout");
            callback(QByteArray(), -1);
        }
        loop.quit();
    });

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()
                         ? reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                         : -1;

    QByteArray responseData = reply->readAll();

    if (reply->error() == QNetworkReply::NoError) {
        callback(responseData, statusCode);
    } else {
        emit requestError(reply->errorString());
        callback(responseData, statusCode);
    }

    reply->deleteLater();
}
//-----------------------------------ASYNC REQUEST-----------------------------------
QNetworkReply* HttpManager::makeRequestAsync(const QUrl &url,
                                             const QJsonDocument &jsonDoc,
                                             const HttpMethod &method)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = nullptr;

    switch (method) {
    case HttpMethod::Get:
        reply = get(request);
        break;
    case HttpMethod::Post:
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = post(request, jsonDoc.toJson());
        break;
    default:
        emit requestError("Unknown HTTP method");
        return nullptr;
    }

    if (reply) {
        QTimer *timer = new QTimer(reply);
        timer->setSingleShot(true);
        timer->setInterval(HTTP_REQUEST_TIMEOUT);
        timer->start();

        connect(timer, &QTimer::timeout, [=]() {
            if (reply->isRunning()) {
                reply->abort();
                emit requestError("Async request timeout");
            }
            reply->deleteLater();
            timer->deleteLater();
        });

        connect(reply, &QNetworkReply::finished, [timer]() { timer->stop(); });
        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), [timer]() { timer->stop(); });

        reply->setProperty("timer", QVariant::fromValue(timer));
    }

    return reply;
}


// void HttpManager::onReplyFinished()
// {
//     timeoutTimer->stop();

//     if (!currentReply) return;

//     int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//     if (currentReply->error() == QNetworkReply::NoError) {
//         currentCallback(currentReply->readAll(), statusCode);
//     } else {
//         emit requestError(currentReply->errorString());
//         currentCallback(QByteArray(), -1);
//     }

//     currentReply->deleteLater();
//     currentReply = nullptr;
//     currentCallback = nullptr;
// }

void HttpManager::onReplyFinished()
{
    timeoutTimer->stop();
    if (!currentReply) return;

    int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = currentReply->readAll();  // Читаем тело ответа всегда

    if (currentReply->error() == QNetworkReply::NoError) {
        currentCallback(responseData, statusCode);
    } else {
        emit requestError(currentReply->errorString());
        currentCallback(responseData, statusCode); // Передаём тело даже при ошибке
    }

    currentReply->deleteLater();
    currentReply = nullptr;
    currentCallback = nullptr;
}

void HttpManager::onReplyErrorOccurred(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    timeoutTimer->stop();
    if (!currentReply) return;
    emit requestError(currentReply->errorString());
    currentCallback(QByteArray(), -1);
    currentReply->deleteLater();
    currentReply = nullptr;
    currentCallback = nullptr;
}

// void HttpManager::onTimeout()
// {
//     if (currentReply) {
//         currentReply->abort();
//         emit requestError("Connection timeout");
//         currentCallback(QByteArray(), -1);
//         currentReply->deleteLater();
//         currentReply = nullptr;
//         currentCallback = nullptr;
//     }
// }

// void HttpManager::onAsyncTimeout()
// {
//     QTimer *timer = qobject_cast<QTimer*>(sender());
//     if (timer) {
//         QNetworkReply *reply = qobject_cast<QNetworkReply*>(timer->parent());
//         if (reply) {
//             if (reply->isRunning()) {
//                 reply->abort();
//                 emit requestError("Async request timeout");
//             }
//             reply->deleteLater();
//         }
//         timer->deleteLater();
//     }
// }
void HttpManager::onAsyncTimeout()
{
    QTimer *timer = qobject_cast<QTimer*>(sender());
    if (!timer) return; // Проверяем, что таймер существует

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(timer->parent());
    if (reply && reply->isRunning()) {
        reply->abort();
        emit requestError("Async request timeout");
    }

    if (reply) reply->deleteLater(); // Удаляем reply
    timer->deleteLater(); // Теперь таймер тоже удалится
}
//-------------------------------CREATE URL-----------------------------------
QUrl HttpManager::createApiUrl(const QString &path)
{
    return QUrl(QString("http://%1:%2/api/v1/%3")
                    .arg(gSettings.getBackendServiceIP())
                    .arg(gSettings.getBackendServicePort())
                    .arg(path));
}
