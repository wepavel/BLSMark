#ifndef MESSAGER_H
#define MESSAGER_H

#include "qobject.h"

class Messager: public QObject {
    Q_OBJECT

public:
    static Messager& instance();
    void sendMessage(const QString& msg);

signals:
    void sendMsg(const QString& msg);

private:
    Messager();
    Messager(const Messager&) = delete;
    Messager& operator=(const Messager&) = delete;
};

#endif // MESSAGER_H
