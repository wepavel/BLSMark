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
    msgView = new MsgLogWidget();
}

void Messager::addMessage(const QString& msg,
                          const MsgType& type,
                          bool shoulShow)
{
    msgView->addMsg(msg, type);
    emit msgWasAdded(type);
    if (shoulShow){
        msgView->showWithOpenTab(type);
    }
}

void Messager::show()
{
    msgView->show();
}

MsgLogWidget *Messager::getView()
{
    return msgView;
}


