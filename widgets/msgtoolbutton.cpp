#include "ui_msgtoolbutton.h"
#include <QStyle>

#include "msgtoolbutton.h"
MsgToolButton::MsgToolButton(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MsgToolButton)
{
    ui->setupUi(this);
    ui->tb_error->setIconSize(QSize(25, 25));
    ui->tb_warning->setIconSize(QSize(25, 25));
}

void MsgToolButton::setMessageCount(const int& notificationCount,
                                    MsgType type)
{
    if (notificationCount < 0)
        return;

    switch (type) {
    case MsgType::Info:
        break;
    case MsgType::Warning:
    {
        this->warningCount = notificationCount;

        if(notificationCount == 0){
            ui->tb_warning->setText("");
            ui->tb_warning->setIcon(QIcon(":/images/img/nowarn.png"));
        }

        if(notificationCount > 0){
            ui->tb_warning->setIcon(QIcon(":/images/img/warn.png"));
            ui->tb_warning->setText(QString::number(notificationCount));
        }

        if(notificationCount > 9){
            ui->tb_warning->setMaximumWidth(60);
            ui->tb_warning->setMinimumWidth(60);
        }  else {
            ui->tb_warning->setMaximumWidth(50);
            ui->tb_warning->setMinimumWidth(50);
        }
        break;
    }
    case MsgType::Error:
    {
        this->errorCount = notificationCount;

        if(errorCount == 0){
            ui->tb_error->setText("");
            ui->tb_error->setIcon(QIcon(":/images/img/noerror.png"));
        }

        if(errorCount > 0){
            ui->tb_error->setIcon(QIcon(":/images/img/no.png"));
            ui->tb_error->setText(QString::number(errorCount));
        }

        if(notificationCount > 9){
            ui->tb_error->setMaximumWidth(60);
            ui->tb_error->setMinimumWidth(60);
        } else {
            ui->tb_error->setMaximumWidth(50);
            ui->tb_error->setMinimumWidth(50);
        }
        break;
    }
    default:
        break;
    }
}

void MsgToolButton::incrementMessageCount(MsgType type)
{
    switch (type) {
    case MsgType::Info:
        break;
    case MsgType::Warning:
    {
        setMessageCount(this->warningCount+1, MsgType::Warning);
        break;
    }
    case MsgType::Error:
    {
        setMessageCount(this->errorCount+1, MsgType::Error);
        break;
    }
    default:
        break;
    }
}

void MsgToolButton::clearAllMsgType(MsgType type)
{
    setMessageCount(0, type);
}

void MsgToolButton::clearAll()
{
    setMessageCount(0, Warning);
    setMessageCount(0, Error);
}

MsgToolButton::~MsgToolButton()
{
    delete ui;
}

void MsgToolButton::on_tb_warning_clicked()
{
    emit tb_clicked(MsgType::Warning);
}

void MsgToolButton::on_tb_error_clicked()
{
    emit tb_clicked(MsgType::Error);
}

void MsgToolButton::on_tb_info_clicked()
{
    emit tb_clicked(MsgType::Info);
}

