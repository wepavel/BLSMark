#include "messager.h"
#include "qmutex.h"

Messager &Messager::instance()
{
    static QMutex mutex;
    static Messager* instance = nullptr;

    if (!instance) {
        QMutexLocker locker(&mutex);
        if (!instance) {
            instance = new Messager();
        }
    }
    return *instance;
}

Messager::Messager() {

}

void Messager::sendMessage(const QString& msg) {
    emit sendMsg(msg);
}
