#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include "healthchecker.h"

HealthChecker::HealthChecker(QObject *parent)
    : QObject{parent}
{
    initMaps();
    connect(&gSettings, &GlobalSettings::backendServiceIpPortChanged, this, &HealthChecker::on_backend_service_ip_port_changed);
    // http
    QString url = QString("http://%1:%2/api/v1/heartbeat/service-heartbeat/ping")
                        .arg(gSettings.getBackendServiceIP())
                        .arg(gSettings.getBackendServicePort());
    httpPingRequest = QNetworkRequest(QUrl(url));
    httpManager = new QNetworkAccessManager(this);
    httpTimer = new QTimer(this);
    httpTimer->setInterval(HTTP_REQUEST_INTERVAL_MS);
    connect(httpTimer, &QTimer::timeout, this, &HealthChecker::httpSendPingRequest);
    httpTimer->start();

    // websocket
    m_websocketUrl = QString("ws://%1:%2/api/v1/streaming/ws-status/1")
                         .arg(gSettings.getBackendServiceIP())
                         .arg(gSettings.getBackendServicePort());
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &HealthChecker::on_ws_connected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &HealthChecker::on_ws_disconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &HealthChecker::on_ws_textMessageReceived);
    m_webSocketReconnectTimer = new QTimer(this);
    m_webSocketRequestTimer = new QTimer(this);
    connect(m_webSocketReconnectTimer, &QTimer::timeout, this, &HealthChecker::wsConnectToServer);
    connect(m_webSocketRequestTimer, &QTimer::timeout, this, &HealthChecker::wsRequestStatesData);
    m_reconnectDebugTimer = new QTimer(this);
    connect(m_reconnectDebugTimer, &QTimer::timeout, this, [this](){
        //qDebug() << "Attempting to connect to server with url " << m_websocketUrl;
        messagerInst.addMessage(QString("WS: Attempting to connect to server with url "+m_websocketUrl), Warning);
    });

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
                emit deviceAvailableChanged("Сервер", true);
                if(lastServiceAvailMsg != "доступен")
                    messagerInst.addMessage("HTTP: Сервер доступен!", Info);
                lastServiceAvailMsg = "доступен";
            } else {
                //qDebug() << "Unexpected response:" << responseData;
                emit deviceAvailableChanged("Сервер", false);
                if(lastServiceAvailMsg != "не доступен")
                    messagerInst.addMessage("HTTP: Сервер недоступен!", Error);
                lastServiceAvailMsg = "не доступен";
            }
        } else {
            //qDebug() << "Error:" << reply->errorString();
            emit deviceAvailableChanged("Сервер", false);
            if(lastServiceAvailMsg != "не доступен")
                messagerInst.addMessage("HTTP: Сервер недоступен!", Error);
            lastServiceAvailMsg = "не доступен";
        }

        // Удаляем объект ответа после завершения работы с ним
        reply->deleteLater();
        httpIsRequestInProgress = false;
    });
}

QWebSocket *HealthChecker::getConnection()
{
    return m_webSocket;
}

void HealthChecker::wsConnectToServer()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {

        //qDebug() << "Attempting to connect to server with url " << m_websocketUrl;
        //messagerInst.addMessage(QString("WS: Attempting to connect to server with url "+m_websocketUrl), Warning);
        m_webSocket->open(m_websocketUrl);
    }
}

void HealthChecker::wsDisconnectFromServer()
{
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Disconnecting from server...";
        messagerInst.addMessage("WS: Disconnecting from server...", Info);
        m_webSocket->close();
    }
}

void HealthChecker::wsRequestStatesData()
{
    QJsonObject message;
    message["type"] = "heartbeat";
    QJsonDocument doc(message);
    QString jsonMessage = doc.toJson(QJsonDocument::Compact);

    m_webSocket->sendTextMessage(jsonMessage);
}

QString HealthChecker::getName(QString name)
{
    return devNamesMap[name];
}

void HealthChecker::initMaps()
{
    devAvailMap["printer"] = false;
    devAvailMap["plc"] = false;
    devAvailMap["scanner"] = false;

    devWorksMap["printer"] = false;
    devWorksMap["plc"] = false;
    devWorksMap["scanner"] = false;

    eventsHandlerMap = {
        {"heartbeat", std::bind(&HealthChecker::processHeartbeatEvent, this, std::placeholders::_1)},
        {"applicator_state", std::bind(&HealthChecker::processApplicationStateEvent, this, std::placeholders::_1)}
    };
}

void HealthChecker::processHeartbeatEvent(const QJsonObject &data)
{
    QJsonArray messagesArray = data.value("message").toArray();

    for (const QJsonValue &value : messagesArray) {
        QJsonObject messageObj = value.toObject();
        QString name = messageObj.value("name").toString();
        bool ping = messageObj.value("ping").toBool();
        bool heartbeat = messageObj.value("heartbeat").toBool();

        emit deviceAvailableChanged(getName(name), ping);
        emit deviceWorksChanged(getName(name), heartbeat);
        if(devAvailMap[name] != ping)
        {
            devAvailMap[name] = ping;
            // emit deviceAvailableChanged(getName(name), ping);
            messagerInst.addMessage(QString("WS: Доступность устройства " +
                                            getName(name).toUpper() +
                                            " изменилась на " +
                                            QString(ping == true ? "<ДОСТУПЕН>" : "<НЕ ДОСТУПЕН>")),
                                    Info);
        }

        if(devWorksMap[name] != heartbeat){
            devWorksMap[name] = heartbeat;
            // emit deviceWorksChanged(getName(name), heartbeat);
            messagerInst.addMessage(QString("WS: Работоспособность устройства " +
                                            getName(name).toUpper() +
                                            " изменилась на " +
                                            QString(heartbeat == true ? "<РАБОТАЕТ>" : "<НЕ РАБОТАЕТ>")),
                                    Info);
        }
    }

    deviceWorksChanged("Сервер", true);
}

void HealthChecker::processApplicationStateEvent(const QJsonObject &data)
{
    emit sendApplicatorStateData(data);
}

void HealthChecker::on_ws_connected()
{
    m_webSocketReconnectTimer->stop();
    m_reconnectDebugTimer->stop();
    m_webSocketRequestTimer->start(WS_REQUEST_INTERVAL_MS);
    emit deviceWorksChanged("Сервер", true);
    //qDebug() << "Connected to server!";
    messagerInst.addMessage("WS: Connected to server!", Info);
}

void HealthChecker::on_ws_disconnected()
{
    qDebug() << "Disconnected from server!";
    messagerInst.addMessage("WS: Disconnected from server!", Warning);
    emit deviceWorksChanged("Сервер", false);
    messagerInst.addMessage("Сервер не работает!", Error);
    m_webSocketRequestTimer->stop();
    // Попытаться переподключиться через 3 секунды
    m_webSocketReconnectTimer->start(WS_RECONNECT_INTERVAL_MS);
    m_reconnectDebugTimer->start(WS_RECONNECT_DEBUG_INTERVAL_MS);
    messagerInst.addMessage(QString("WS: Attempting to connect to server with url "+m_websocketUrl), Warning);
}

void HealthChecker::on_ws_textMessageReceived(const QString &message)
{
    //qDebug() << message;
    // Шаг 1: Десериализуем строку JSON в QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    // Проверим, был ли JSON корректно распарсен
    if (doc.isNull()) {
        qDebug() << "Invalid JSON format!";
        messagerInst.addMessage("WS: on_ws_textMessageReceived::Invalid JSON format!", Error);
        return;
    }

    // Шаг 2: Извлекаем объект данных
    QJsonObject rootObj = doc.object();
    QString event = rootObj.value("event").toString();
    QJsonObject data = rootObj.value("data").toObject();

    // вызываем функцию из eventsHandlerMap в зависимости от типа события
    // с передачей в нее данных
    eventsHandlerMap[event](data);
}

void HealthChecker::on_backend_service_ip_port_changed()
{
    // Обновление url для http
    QString url = QString("http://%1:%2/api/v1/heartbeat/service-heartbeat/ping")
                                            .arg(gSettings.getBackendServiceIP())
                                            .arg(gSettings.getBackendServicePort());
    httpPingRequest = QNetworkRequest(QUrl(url)); // обновляем url
    httpIsRequestInProgress = false;
    httpSendPingRequest();

    // Обновление и переподключение для ws
    m_websocketUrl = QString("ws://%1:%2/api/v1/streaming/ws-status/1")
                         .arg(gSettings.getBackendServiceIP())
                         .arg(gSettings.getBackendServicePort());
    //qDebug() << "newwsurl" << m_websocketUrl;
    wsDisconnectFromServer();
    wsConnectToServer();
}

HealthChecker::~HealthChecker()
{
    m_webSocketReconnectTimer->stop();
    m_webSocketRequestTimer->stop();
    m_reconnectDebugTimer->stop();
    wsDisconnectFromServer();
    delete httpManager;
    delete httpTimer;
    delete m_webSocketReconnectTimer;
    delete m_webSocketRequestTimer;
    delete m_reconnectDebugTimer;
    delete m_webSocket;
}
