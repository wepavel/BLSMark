#include "healthchecker.h"
#include "qthread.h"

HealthChecker::HealthChecker(QObject *parent)
    : QObject{parent}
{
    ping_request = QNetworkRequest(QUrl("http://127.0.0.1:8001/api/v1/heartbeat/service-heartbeat/ping"));
    manager = new QNetworkAccessManager(this);
    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &HealthChecker::sendPingRequest);
    timer->start();
}

void HealthChecker::sendPingRequest()
{
    // Отправляем GET запрос
    QNetworkReply *reply = manager->get(ping_request);

    // Ожидаем завершения запроса (это не блокирует основной поток)
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        // Получаем ответ от сервера
        QByteArray responseData = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Response:" << responseData;

            // Проверяем, что в ответе есть "pong"
            if (responseData == "pong") {
                qDebug() << "Received pong!";
                emit serviceIsAvailable();
            } else {
                qDebug() << "Unexpected response:" << responseData;
                emit serviceIsNotAvailable();
            }
        } else {
            qDebug() << "Error:" << reply->errorString();
            emit serviceIsNotAvailable();
        }

        // Удаляем объект ответа после завершения работы с ним
        reply->deleteLater();
    });
}

HealthChecker::~HealthChecker()
{
    delete manager;
    delete timer;
}
