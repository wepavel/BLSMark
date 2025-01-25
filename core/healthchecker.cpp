#include "healthchecker.h"
#include "qthread.h"

HealthChecker::HealthChecker(QObject *parent)
    : QObject{parent}
{
    // http
    httpPingRequest = QNetworkRequest(QUrl("http://127.0.0.1:8001/api/v1/heartbeat/service-heartbeat/ping"));
    httpManager = new QNetworkAccessManager(this);
    httpTimer = new QTimer(this);
    httpTimer->setInterval(500);
    connect(httpTimer, &QTimer::timeout, this, &HealthChecker::httpSendPingRequest);
    httpTimer->start();

    // websocket
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &HealthChecker::on_ws_connected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &HealthChecker::on_ws_disconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &HealthChecker::on_ws_textMessageReceived);
    m_webSocketReconnectTimer = new QTimer(this);
    connect(m_webSocketReconnectTimer, &QTimer::timeout, this, &HealthChecker::wsConnectToServer);
    wsConnectToServer();
}

void HealthChecker::httpSendPingRequest()
{
    if (httpIsRequestInProgress)
        return;

    httpIsRequestInProgress = true;

    // Отправляем GET запрос
    QNetworkReply *reply = httpManager->get(httpPingRequest);

    // Ожидаем завершения запроса (это не блокирует основной поток)
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        // Получаем ответ от сервера
        QByteArray responseData = reply->readAll();
        if (reply->error() == QNetworkReply::NoError) {
            //qDebug() << "Response:" << responseData;

            // Проверяем, что в ответе есть "pong"
            if (responseData == "pong") {
                //qDebug() << "Received pong!";
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
        httpIsRequestInProgress = false;
    });
}

void HealthChecker::wsConnectToServer()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Attempting to connect to server...";
        m_webSocket->open(m_websocketUrl);
    }
}

void HealthChecker::on_ws_connected()
{
    m_webSocketReconnectTimer->stop();
    emit serviceWorks();
    qDebug() << "Connected to server!";
}

void HealthChecker::on_ws_disconnected()
{
    qDebug() << "Disconnected from server!";
    emit serviceDoesNotWork();
    // Попытаться переподключиться через 3 секунды
    m_webSocketReconnectTimer->start(WS_RECONNECT_INTERVAL_MS);
}

void HealthChecker::on_ws_textMessageReceived(const QString &message)
{
    qDebug() << "Received message:" << message;
}

HealthChecker::~HealthChecker()
{
    delete httpManager;
    delete httpTimer;
    delete m_webSocketReconnectTimer;
    delete m_webSocket;
}
