#include "messager.h"

Messager &Messager::instance()
{
    static Messager instance;
    return instance;
}

Messager::Messager() {

}

void Messager::sendMessage(const QString& msg) {
    emit sendMsg(msg);
}
