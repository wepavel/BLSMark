#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <qapplication.h>
#include "wsmanager.h"
#include "qmessagebox.h"

WsManager::WsManager(QObject *parent)
    : QObject{parent}
{
    initMaps();
    connect(&gSettings, &GlobalSettings::backendServiceIpPortChanged, this, &WsManager::on_backend_service_ip_port_changed);
    // http
    QString url = QString("http://%1:%2/api/v1/heartbeat/service-heartbeat/ping")
                        .arg(gSettings.getBackendServiceIP())
                        .arg(gSettings.getBackendServicePort());
    httpPingRequest = QNetworkRequest(QUrl(url));
    httpManager = new QNetworkAccessManager(this);
    httpTimer = new QTimer(this);
    httpTimer->setInterval(HTTP_REQUEST_INTERVAL_MS);
    connect(httpTimer, &QTimer::timeout, this, &WsManager::httpSendPingRequest);
    httpTimer->start();

    //qDebug() << gSettings.getClientId() << gSettings.getClientId() << gSettings.getClientId();
    // websocket
    m_websocketUrl = QString("ws://%1:%2/api/v1/streaming/ws-status/%3")
                         .arg(gSettings.getBackendServiceIP())
                         .arg(gSettings.getBackendServicePort())
                         .arg(gSettings.getClientId());
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &WsManager::on_ws_connected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WsManager::on_ws_disconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WsManager::on_ws_textMessageReceived);
    m_webSocketReconnectTimer = new QTimer(this);
    m_webSocketRequestTimer = new QTimer(this);
    connect(m_webSocketReconnectTimer, &QTimer::timeout, this, &WsManager::wsConnectToServer);
    connect(m_webSocketRequestTimer, &QTimer::timeout, this, &WsManager::wsRequestStatesData);
    m_reconnectDebugTimer = new QTimer(this);
    connect(m_reconnectDebugTimer, &QTimer::timeout, this, [this](){
        //qDebug() << "Attempting to connect to server with url " << m_websocketUrl;
        messagerInst.addMessage(QString("WS: Attempting to connect to server with url "+m_websocketUrl), Warning);
    });

    wsConnectToServer();
}


void WsManager::httpSendPingRequest()
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

QWebSocket *WsManager::getConnection()
{
    return m_webSocket;
}

void WsManager::wsConnectToServer()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {

        //qDebug() << "Attempting to connect to server with url " << m_websocketUrl;
        //messagerInst.addMessage(QString("WS: Attempting to connect to server with url "+m_websocketUrl), Warning);
        m_webSocket->open(m_websocketUrl);
    }
}

void WsManager::wsDisconnectFromServer()
{
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Disconnecting from server...";
        messagerInst.addMessage("WS: Disconnecting from server...", Info);
        m_webSocket->close();
    }
}

void WsManager::wsRequestStatesData()
{
    QJsonObject message;
    message["type"] = "heartbeat";
    QJsonDocument doc(message);
    QString jsonMessage = doc.toJson(QJsonDocument::Compact);

    m_webSocket->sendTextMessage(jsonMessage);
}

QString WsManager::getName(QString name)
{
    return devNamesMap[name];
}

void WsManager::initMaps()
{
    devAvailMap["printer"] = false;
    devAvailMap["plc"] = false;
    devAvailMap["scanner"] = false;

    devWorksMap["printer"] = false;
    devWorksMap["plc"] = false;
    devWorksMap["scanner"] = false;

    eventsHandlerMap = {
        {"heartbeat", std::bind(&WsManager::processHeartbeatEvent, this, std::placeholders::_1)},
        {"applicator_state", std::bind(&WsManager::processApplicationStateEvent, this, std::placeholders::_1)},
        {"broadcast_messagebox", std::bind(&WsManager::processMessageBoxEvent, this, std::placeholders::_1)}
    };
}

void WsManager::processHeartbeatEvent(const QJsonObject &data)
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

void WsManager::processApplicationStateEvent(const QJsonObject &data)
{
    emit sendApplicatorStateData(data);
}

void WsManager::processMessageBoxEvent(const QJsonObject &data)
{
    QString message = data["message"].toString();
    QString notificationType = data["notification_type"].toString();

    QMessageBox msgBox;
    QString windowTitle = "";

    if (notificationType == "WARNING"){
        msgBox.setIcon(QMessageBox::Warning);
        windowTitle = "Внимание!";
        messagerInst.addMessage(QString("Сообщение от сервера: %1").arg(message), Warning);
    }

    if (notificationType == "CRITICAL"){
        msgBox.setIcon(QMessageBox::Critical);
        windowTitle = "Критическая ошибка!";
        messagerInst.addMessage(QString("Сообщение от сервера: %1").arg(message), Error);
    }

    if (notificationType == "SUCCESS"){
        msgBox.setIconPixmap(QIcon(":/images/img/success.svg").pixmap(32, 32));
        windowTitle = "Успех!";
        messagerInst.addMessage(QString("Сообщение от сервера: %1").arg(message), Success);
    }

    if (notificationType == "INFO"){
        msgBox.setIcon(QMessageBox::Information);
        windowTitle = "Информация";
        messagerInst.addMessage(QString("Сообщение от сервера: %1").arg(message), Info);
    }

    msgBox.setWindowTitle(windowTitle);

    // Установка родителя (если есть активное окно, QMessageBox появится в его центре)
    QWidget* parentWidget = qobject_cast<QWidget*>(QApplication::activeWindow());
    if (parentWidget) {
        msgBox.setParent(parentWidget);
        msgBox.setWindowModality(Qt::WindowModal); // Блокирует только родительское окно
    }

    msgBox.setText(QString("Сообщение от сервера: %1").arg(message));

    QPushButton *okButton = msgBox.addButton("Ок", QMessageBox::AcceptRole);
    msgBox.setDefaultButton(okButton);

    msgBox.exec();
}

void WsManager::on_ws_connected()
{
    m_webSocketReconnectTimer->stop();
    m_reconnectDebugTimer->stop();
    m_webSocketRequestTimer->start(WS_REQUEST_INTERVAL_MS);
    emit deviceWorksChanged("Сервер", true);
    //qDebug() << "Connected to server!";
    messagerInst.addMessage("WS: Connected to server!", Info);
}

void WsManager::on_ws_disconnected()
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

void WsManager::on_ws_textMessageReceived(const QString &message)
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
    if (eventsHandlerMap.contains(event)){
        eventsHandlerMap[event](data);
    }
}

void WsManager::on_backend_service_ip_port_changed()
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

WsManager::~WsManager()
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
