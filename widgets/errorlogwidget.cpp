#include "errorlogwidget.h"
#include "ui_errorlogwidget.h"
#include "core/messager.h"

ErrorLogWidget::ErrorLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ErrorLogWidget)
{
    ui->setupUi(this);
}

ErrorLogWidget::~ErrorLogWidget()
{
    delete ui;
}

void ErrorLogWidget::on_pb_clear_clicked()
{
    ui->lw_output->clear();
    emit dataHasBeenCleared();
}

void ErrorLogWidget::addMsg(const QString &msg)
{
    qDebug() << "Добавляем сообщение: " << msg;
    ui->lw_output->addItem(msg);
}

