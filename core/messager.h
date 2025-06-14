#ifndef MESSAGER_H
#define MESSAGER_H

#include "qobject.h"
#include "widgets/msglogwidget.h"

#define messagerInst Messager::instance()

class Messager: public QObject {
    Q_OBJECT

public:
    static Messager& instance();
    void destroyUI();
    void addMessage(const QString& msg,
                    const MsgType& type,
                    bool shoulShow = false);
    void show();
    MsgLogWidget* getView();

private:
    Messager();
    MsgLogWidget* msgView = nullptr;
    Messager(const Messager&) = delete;
    Messager& operator=(const Messager&) = delete;

signals:
    void msgWasAdded(MsgType type);
};

#endif // MESSAGER_H
