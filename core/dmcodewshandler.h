#ifndef DMCODEWSHANDLER_H
#define DMCODEWSHANDLER_H

#include <QObject>
#include "QWebSocket"

class DmCodeWsHandler : public QObject
{
    Q_OBJECT
public:
    explicit DmCodeWsHandler(QWebSocket* socket,
                             QObject *parent = nullptr);

private:
    QWebSocket* m_socket;

private slots:
    void on_dm_code_received(const QString &message);

signals:
    void dmCodeReceived(const QString& message);
};

#endif // DMCODEWSHANDLER_H
