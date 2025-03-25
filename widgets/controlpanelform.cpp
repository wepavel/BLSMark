#include "controlpanelform.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qmessagebox.h"
#include "ui_controlpanelform.h"

ControlPanelForm::ControlPanelForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlPanelForm)
{
    ui->setupUi(this);
    ui->cb_gtin_names->setFocusPolicy(Qt::NoFocus);
    ui->pb_start->setFocusPolicy(Qt::NoFocus);
    setSystemState(Stopped);
    httpManager = new HttpManager(this);
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
    //qDebug() << "setSystemState: " << state;
    switch (state) {
    case SystemState::Working:
    {
        ui->lbl_system_state->setText("в работе");
        ui->lbl_system_state->setStyleSheet("color: green");
        ui->cb_gtin_names->setEnabled(false);
        currentState = Working;
        break;
    }
    case SystemState::Stopped:
    {
        ui->lbl_system_state->setText("остановлена");
        ui->lbl_system_state->setStyleSheet("color: red");
        ui->cb_gtin_names->setEnabled(true);
        currentState = Stopped;
        break;
    }
    default:
        break;
    }
}

void ControlPanelForm::setCurrentProduct(const QString &product)
{
    if(product.isEmpty()){
        ui->lbl_currentProduct->setText("пусто");
        ui->lbl_currentProduct->setStyleSheet("color: red;");
        return;
    }
    ui->lbl_currentProduct->setText(product);
    ui->lbl_currentProduct->setStyleSheet("color: green;");
}

void ControlPanelForm::setControlPanelActive(const bool &active)
{
    if(active == false){
        setSystemState(Stopped);
        setCurrentProduct("");
        setProductsLeftCount(0);
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

void ControlPanelForm::serverWorksChanged(QString devName, bool works)
{
    //qDebug() << devName << "works: " << works;

    if(devName!="Сервер"){
        return;
    }

    setControlPanelActive(works);
}

void ControlPanelForm::serverAvailableChanged(QString devName, bool available)
{
    //qDebug() << devName << "available: " << available;

    if(devName!="Сервер"){
        return;
    }

    setControlPanelActive(available);
}


void ControlPanelForm::on_pb_start_clicked()
{
    if(ui->cb_gtin_names->currentText().isEmpty()){
        QMessageBox::warning(this,
                             "Внимание",
                             "Позиция на печать не выбрана, пожалуйста, выберите позицию",
                             QMessageBox::Ok);
        return;
    }
    ui->pb_start->setEnabled(false);
    ui->pb_stop->setEnabled(false);
    ui->cb_gtin_names->setEnabled(false);
    QUrl url = HttpManager::createApiUrl(QString("code-process/set-system-working/%1")
                                             .arg(ui->cb_gtin_names->getGtin()));
    httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){
        if (statusCode!=200 && statusCode!=-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_start->setEnabled(true);
            ui->pb_stop->setEnabled(true);
        } else if (statusCode==-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_start->setEnabled(true);
            ui->pb_stop->setEnabled(true);
        } else {
            ui->pb_start->setEnabled(true);
            ui->pb_stop->setEnabled(true);
            setSystemState(Working);
        }
    });
}


void ControlPanelForm::on_pb_stop_clicked()
{
    ui->pb_stop->setEnabled(false);
    ui->pb_start->setEnabled(false);
    QUrl url = HttpManager::createApiUrl(QString("code-process/set-system-stop"));
    httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){
        if (statusCode!=200 && statusCode!=-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-stop/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
        } else if (statusCode==-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-stop/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
        } else {
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
            setSystemState(Stopped);
        }
    });
}

