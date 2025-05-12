#ifndef WSMANAGER_H
#define WSMANAGER_H

#include "qtimer.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>
#include "globalsettings.h"
#include "messager.h"

#define HTTP_REQUEST_INTERVAL_MS 500
#define WS_RECONNECT_INTERVAL_MS 3000
#define WS_RECONNECT_DEBUG_INTERVAL_MS 10000
#define WS_REQUEST_INTERVAL_MS 2000

class WsManager : public QObject
{
    Q_OBJECT
public:
    explicit WsManager(QObject *parent = nullptr);
    ~WsManager();

    void httpSendPingRequest();
    QWebSocket* getConnection();

signals:
    void deviceAvailableChanged(QString devName, bool available);
    void deviceWorksChanged(QString devName, bool works);

    void sendApplicatorStateData(const QJsonObject& data);

private slots:
    void on_ws_connected();
    void on_ws_disconnected();
    void on_ws_textMessageReceived(const QString &message);
    void on_backend_service_ip_port_changed();

private:
    // HTTP
    QNetworkAccessManager *httpManager;
    QNetworkRequest httpPingRequest;
    QTimer* httpTimer;
    bool httpIsRequestInProgress = false;

    // WebSocket
    QWebSocket *m_webSocket;
    QString m_websocketUrl;
    QTimer *m_webSocketReconnectTimer;
    QTimer *m_webSocketRequestTimer;
    QTimer *m_reconnectDebugTimer;
    bool wsIsRequestInProgress = false;

    void wsConnectToServer();
    void wsDisconnectFromServer();
    void wsRequestStatesData();

    QString getName(QString name);

    QMap<QString, QString> devNamesMap = {
        {"printer","Принтер"},
        {"plc","Аппликатор"},
        {"scanner","Сканер"},
        {"database","База данных"},
    };

    /**
     * @brief eventsHandlerMap
     * ключ - тип события
     * значение - функция, которая вызывается для обработки данных QJsonObject
     * этого события
     */
    QMap<QString, std::function<void(const QJsonObject &)>> eventsHandlerMap;

    QString lastServiceAvailMsg = "";

    // map для хранения последнего состояния доступности устройств
    QMap<QString, bool> devAvailMap;

    // map для хранения последнего состояния работы устройств
    QMap<QString, bool> devWorksMap;

    void initMaps();

    /// ___________________ОБРАБОТЧИКИ СОБЫТИЙ WEBSOCKET_____________________
    void processHeartbeatEvent(const QJsonObject& data);
    void processApplicationStateEvent(const QJsonObject& data);
    void processMessageBoxEvent(const QJsonObject& data);
};

#endif // WSMANAGER_H
