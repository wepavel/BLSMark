#ifndef ERRORTOOLBUTTON_H
#define ERRORTOOLBUTTON_H

#include <QWidget>

namespace Ui {
class ErrorToolButton;
}

class ErrorToolButton : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorToolButton(const int& notificationCount, QWidget *parent = nullptr);
    void setNotificationCount(const int& notificationCount);
    void incrementNotificationCount();
    void clearAllNotifications();
    ~ErrorToolButton();

private:
    Ui::ErrorToolButton *ui;
    int notificationCount = 0;

signals:
    void tb_clicked();

private slots:
    void on_toolButton_clicked();
};

#endif // ERRORTOOLBUTTON_H
