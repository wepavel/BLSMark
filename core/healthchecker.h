#ifndef HEALTHCHECKER_H
#define HEALTHCHECKER_H

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
#define WS_REQUEST_INTERVAL_MS 2000

class HealthChecker : public QObject
{
    Q_OBJECT
public:
    explicit HealthChecker(QObject *parent = nullptr);
    ~HealthChecker();

    void httpSendPingRequest();
    QWebSocket* getConnection();

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
    bool wsIsRequestInProgress = false;

    void wsConnectToServer();
    void wsDisconnectFromServer();
    void wsRequestStatesData();

    QString getName(QString name);
    QMap<QString, QString> devNamesMap = {
        {"printer","Принтер"},
        {"plc","ПЛК"},
        {"scanner","Сканер"},
    };

private slots:
    void on_ws_connected();
    void on_ws_disconnected();
    void on_ws_textMessageReceived(const QString &message);
    void on_backend_service_ip_port_changed();

signals:
    void deviceAvailableChanged(QString devName, bool available);
    void deviceWorksChanged(QString devName, bool works);
};

#endif // HEALTHCHECKER_H
