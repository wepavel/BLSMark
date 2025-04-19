#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkReply>

/**
 * @brief Класс для управления HTTP-запросами.
 *
 * Предоставляет методы для выполнения HTTP-запросов и обработки ответов.
 */
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


    QNetworkReply* makeRequestAsync(const QUrl &url,
                                    const QJsonDocument &jsonDoc,
                                    const HttpMethod &method);

    static QUrl createApiUrl(const QString &path);

signals:
    void requestError(const QString &errorMessage);

private slots:
    void onReplyFinished();
    void onReplyErrorOccurred(QNetworkReply::NetworkError error);
    // void onTimeout();
    void onAsyncTimeout();

private:
    std::function<void(const QByteArray&, int)> currentCallback;
    QNetworkReply *currentReply;
    QTimer *timeoutTimer;
};

#endif // HTTPMANAGER_H
