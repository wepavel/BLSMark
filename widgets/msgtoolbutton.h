#ifndef MSGTOOLBUTTON_H
#define MSGTOOLBUTTON_H

#include <QWidget>

enum MsgType{
    Error,
    Warning,
    Info,
    Success
};

namespace Ui {
class MsgToolButton;
}

class MsgToolButton : public QWidget
{
    Q_OBJECT

public:
    explicit MsgToolButton(QWidget *parent = nullptr);
    void setMessageCount(const int& notificationCount, MsgType type);
    void incrementMessageCount(MsgType type);
    void clearAllMsgType(MsgType type);
    void clearAll();
    ~MsgToolButton();

private:
    Ui::MsgToolButton *ui;
    int errorCount = 0;
    int warningCount = 0;

signals:
    void tb_clicked(MsgType type);

private slots:
    void on_tb_warning_clicked();
    void on_tb_error_clicked();
    void on_tb_info_clicked();
};

#endif // MSGTOOLBUTTON_H
