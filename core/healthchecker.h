#ifndef HEALTHCHECKER_H
#define HEALTHCHECKER_H

#include "qtimer.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


class HealthChecker : public QObject
{
    Q_OBJECT
public:
    explicit HealthChecker(QObject *parent = nullptr);
    ~HealthChecker();

    void sendPingRequest();

private:
    QNetworkAccessManager *manager;
    QNetworkRequest ping_request;
    QTimer* timer;
    bool isRequestInProgress = false;

signals:
    void serviceIsAvailable();
    void serviceIsNotAvailable();
};

#endif // HEALTHCHECKER_H
