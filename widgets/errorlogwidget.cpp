#include "errorlogwidget.h"
#include "ui_errorlogwidget.h"
#include "core/messager.h"

ErrorLogWidget::ErrorLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ErrorLogWidget)
{
    ui->setupUi(this);
    connect(&Messager::instance(), &Messager::sendMsg, this, &ErrorLogWidget::addMsg);
}

ErrorLogWidget::~ErrorLogWidget()
{
    delete ui;
}

void ErrorLogWidget::on_pb_clear_clicked()
{
    ui->lw_output->clear();
}

void ErrorLogWidget::addMsg(const QString &msg)
{
    ui->lw_output->addItem(msg);
}

