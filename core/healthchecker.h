#ifndef HEALTHCHECKER_H
#define HEALTHCHECKER_H

#include "qtimer.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>

#define WS_RECONNECT_INTERVAL_MS 3000

class HealthChecker : public QObject
{
    Q_OBJECT
public:
    explicit HealthChecker(QObject *parent = nullptr);
    ~HealthChecker();

    void httpSendPingRequest();

private:
    // HTTP
    QNetworkAccessManager *httpManager;
    QNetworkRequest httpPingRequest;
    QTimer* httpTimer;
    bool httpIsRequestInProgress = false;

    // WebSocket
    QWebSocket *m_webSocket;
    QString m_websocketUrl = "ws://127.0.0.1:8001/api/v1/heartbeat/ws-status/1";
    QTimer *m_webSocketReconnectTimer;

    void wsConnectToServer();


private slots:
    void on_ws_connected();
    void on_ws_disconnected();
    void on_ws_textMessageReceived(const QString &message);

signals:
    // http
    void serviceIsAvailable();
    void serviceIsNotAvailable();

    // ws
    void serviceWorks();
    void serviceDoesNotWork();
};

#endif // HEALTHCHECKER_H
