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
    errorView = new ErrorLogWidget();
}

void Messager::addMessage(const QString& msg, bool shoulShow)
{
    errorView->addMsg(msg);
    emit errorWasAdded();
    if (shoulShow){
        errorView->show();
    }
}

void Messager::showErrors()
{
    errorView->show();
}

ErrorLogWidget *Messager::getView()
{
    return errorView;
}


