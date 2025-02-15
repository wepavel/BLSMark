#include "errortoolbutton.h"
#include "qstyle.h"
#include "ui_errortoolbutton.h"

ErrorToolButton::ErrorToolButton(const int& notificationCount, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ErrorToolButton)
{
    ui->setupUi(this);
    ui->toolButton->setIconSize(QSize(20, 20));
    setNotificationCount(notificationCount);
}

void ErrorToolButton::setNotificationCount(const int &notificationCount)
{
    this->notificationCount = notificationCount;

    if(notificationCount < 0)
        return;

    if(notificationCount == 0){
        ui->toolButton->setText("");
        ui->toolButton->setIcon(QIcon(":/images/img/yes.png"));
    }

    if(notificationCount > 0){
        ui->toolButton->setIcon(QIcon(":/images/img/no.png"));
        ui->toolButton->setText(QString::number(notificationCount));
    }
}

void ErrorToolButton::incrementNotificationCount()
{
    setNotificationCount(this->notificationCount+1);
}

void ErrorToolButton::clearAllNotifications()
{
    setNotificationCount(0);
}

ErrorToolButton::~ErrorToolButton()
{
    delete ui;
}

void ErrorToolButton::on_toolButton_clicked()
{
    emit tb_clicked();
}

