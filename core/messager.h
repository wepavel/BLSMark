#ifndef MESSAGER_H
#define MESSAGER_H

#include "qobject.h"
#include "widgets/errorlogwidget.h"

class Messager: public QObject {
    Q_OBJECT

public:
    static Messager& instance();
    void addMessage(const QString& msg, bool shoulShow = false);
    void showErrors();
    ErrorLogWidget* getView();

private:
    Messager();
    ErrorLogWidget* errorView = nullptr;
    Messager(const Messager&) = delete;
    Messager& operator=(const Messager&) = delete;

signals:
    void errorWasAdded();
};

#endif // MESSAGER_H
