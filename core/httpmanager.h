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
                     const std::function<void(const QByteArray&, int)> &callback);

    static QUrl createApiUrl(const QString &path);

signals:
    void requestError(const QString& err);

};

#endif // HTTPMANAGER_H
