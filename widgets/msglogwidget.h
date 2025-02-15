#ifndef MSGLOGWIDGET
#define MSGLOGWIDGET

#include "widgets/msgtoolbutton.h"
#include <QWidget>

namespace Ui {
class MsgLogWidget;
}

class MsgLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MsgLogWidget(QWidget *parent = nullptr);
    void addMsg(const QString& msg, MsgType type);
    void showWithOpenTab(const MsgType& type);
    ~MsgLogWidget();

private slots:
    void on_pb_clear_clicked();

signals:
    void dataHasBeenCleared();

private:
    Ui::MsgLogWidget *ui;
};

#endif // MSGLOGWIDGET
