#include "controlpanelform.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "ui_controlpanelform.h"

ControlPanelForm::ControlPanelForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlPanelForm)
{
    ui->setupUi(this);
    ui->cb_gtin_names->setFocusPolicy(Qt::NoFocus);
    ui->pb_start_stop->setFocusPolicy(Qt::NoFocus);
    setSystemState(Stopped);
}

ControlPanelForm::~ControlPanelForm()
{
    delete ui;
}

void ControlPanelForm::setProductsLeftCount(const int &leftCount)
{
    ui->lbl_products_left->setText(QString::number(leftCount));
    if(leftCount > 50){
        ui->lbl_products_left->setStyleSheet("color: green");
    } else {
        ui->lbl_products_left->setStyleSheet("color: red");
    }
}

void ControlPanelForm::setSystemState(const SystemState &state)
{
    switch (state) {
    case SystemState::Working:
    {
        ui->lbl_system_state->setText("в работе");
        ui->lbl_system_state->setStyleSheet("color: green");
        currentState = Working;
        ui->pb_start_stop->setIcon(QIcon(":/images/img/stop.svg"));
        ui->pb_start_stop->setText("Остановка печати");
        break;
    }
    case SystemState::Stopped:
    {
        ui->lbl_system_state->setText("остановлена");
        ui->lbl_system_state->setStyleSheet("color: red");
        currentState = Stopped;
        ui->pb_start_stop->setIcon(QIcon(":/images/img/play.svg"));
        ui->pb_start_stop->setText("Запуск печати");
        break;
    }
    default:
        break;
    }
}

void ControlPanelForm::setCurrentProduct(const QString &product)
{
    ui->lbl_currentProduct->setText(product);
}

void ControlPanelForm::on_pb_start_stop_clicked()
{
    switch (currentState) {
    case SystemState::Working:
    {
        setSystemState(Stopped);
        break;
    }
    case SystemState::Stopped:
    {
        setSystemState(Working);
        break;
    }
    default:
        break;
    }
}

void ControlPanelForm::processWsData(const QJsonObject &data)
{
    QJsonObject msg = data.value("message").toObject();

    QString currentProduct = msg.value("current_product").toString();
    int remainder = msg.value("remainder").toInt();
    bool inWork = msg.value("in_work").toBool();
    if(inWork){
        setSystemState(Working);
    } else {
        setSystemState(Stopped);
    }
    setCurrentProduct(currentProduct);
    setProductsLeftCount(remainder);
    // qDebug() << currentProduct
    //          << remainder
    //          << inWork;
}

