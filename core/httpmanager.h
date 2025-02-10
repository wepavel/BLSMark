#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QNetworkAccessManager>

class HttpManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit HttpManager(QObject *parent = nullptr);

    enum HttpMethod{
        Get,
        Post
    };

    void makeRequest(const QUrl &url,
                     const QJsonDocument &jsonDoc,
                     const HttpMethod &method,
                     const std::function<void(const QByteArray&)> &successCallback);

    static QUrl createApiUrl(const QString &endpoint);

signals:
    void requestError(const QString& err);

};

#endif // HTTPMANAGER_H
