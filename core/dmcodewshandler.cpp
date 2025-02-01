#include "dmcodewshandler.h"
#include "qjsondocument.h"
#include "qjsonobject.h"

DmCodeWsHandler::DmCodeWsHandler(QWebSocket* socket,
                                 QObject *parent)
    : QObject{parent}
{
    m_socket = socket;
    connect(m_socket, &QWebSocket::textMessageReceived, this, &DmCodeWsHandler::on_dm_code_received);
}

void DmCodeWsHandler::on_dm_code_received(const QString &message)
{
    if (!message.contains("dmcode_stream"))
        return;

    emit dmCodeReceived(message);
}
