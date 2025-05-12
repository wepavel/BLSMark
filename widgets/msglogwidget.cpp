#include "msglogwidget.h"
#include "ui_msglogwidget.h"
#include "core/messager.h"

#include <qdatetime.h>

MsgLogWidget::MsgLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MsgLogWidget)
{
    ui->setupUi(this);
}

MsgLogWidget::~MsgLogWidget()
{
    delete ui;
}

void MsgLogWidget::on_pb_clear_clicked()
{
    ui->lw_common->clear();
    ui->lw_errors->clear();
    ui->lw_info->clear();
    ui->lw_warnings->clear();
    emit dataHasBeenCleared();
}

void MsgLogWidget::addMsg(const QString &msg, MsgType type)
{
    //qDebug() << "Добавляем сообщение: " << msg;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    switch (type) {

    case MsgType::Info: {
        ui->lw_info->addItem(dateTime + " INFO: " + msg);
        ui->lw_common->addItem(dateTime + " INFO: " + msg);
        break;
    }

    case MsgType::Warning: {
        QListWidgetItem* warningItem = new QListWidgetItem(dateTime + " WARNING: " + msg);
        warningItem->setForeground(QColor(255, 165, 0)); // Оранжевый цвет для Warning

        QListWidgetItem* warningItem1 = new QListWidgetItem(dateTime + " WARNING: " + msg);
        warningItem1->setForeground(QColor(255, 165, 0)); // Оранжевый цвет для Warning

        ui->lw_warnings->addItem(warningItem);
        ui->lw_common->addItem(warningItem1);
        break;
    }

    case MsgType::Error: {
        QListWidgetItem* errorItem = new QListWidgetItem(dateTime + " ERROR: " + msg);
        errorItem->setForeground(Qt::red); // Красный цвет для Error

        QListWidgetItem* errorItem1 = new QListWidgetItem(dateTime + " ERROR: " + msg);
        errorItem1->setForeground(Qt::red); // Красный цвет для Error

        ui->lw_errors->addItem(errorItem);
        ui->lw_common->addItem(errorItem1);
        break;
    }

    case MsgType::Success: {
        QListWidgetItem* successItem = new QListWidgetItem(dateTime + " SUCCESS: " + msg);
        successItem->setForeground(Qt::green); // Зеленый цвет для Успеха

        QListWidgetItem* successItem1 = new QListWidgetItem(dateTime + " SUCCESS: " + msg);
        successItem1->setForeground(Qt::green);  // Зеленый цвет для Успеха

        ui->lw_info->addItem(successItem);
        ui->lw_common->addItem(successItem1);
        break;
    }

    default:
        break;
    }
}

void MsgLogWidget::showWithOpenTab(const MsgType &type)
{
    switch (type) {
    case MsgType::Info:
    {
        ui->tabWidget->setCurrentIndex(0);
        break;
    }
    case MsgType::Success:
    {
        ui->tabWidget->setCurrentIndex(0);
        break;
    }
    case MsgType::Warning:
    {
        ui->tabWidget->setCurrentIndex(2);
        break;
    }
    case MsgType::Error:
    {
        ui->tabWidget->setCurrentIndex(1);
        break;
    }
    default:
        break;
    }
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    show();
    raise();
    activateWindow();
}

