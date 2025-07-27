#include "controlpanelform.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qmessagebox.h"
#include "ui_controlpanelform.h"

ControlPanelForm::ControlPanelForm(HealthCheckForm *hf, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlPanelForm)
{
    ui->setupUi(this);
    this->healthForm = hf;
    ui->cb_gtin_names->setFocusPolicy(Qt::NoFocus);
    ui->pb_start->setFocusPolicy(Qt::NoFocus);
    ui->pb_clean_history->setFocusPolicy(Qt::NoFocus);
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

QPushButton *ControlPanelForm::getClearHistoryPbPtr()
{
    return ui->pb_clean_history;
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
    //qDebug() << currentProduct
    //         << remainder
    //         << inWork;
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
    if (ui->cb_gtin_names->currentText().isEmpty()){
        QMessageBox::warning(this,
                             "Внимание",
                             "Позиция на печать не выбрана, пожалуйста, выберите позицию",
                             QMessageBox::Ok);
        return;
    }

    // bool hasUnavailable = !this->healthForm->getNotAvailUnits().isEmpty();
    // bool hasNotWorking = !this->healthForm->getNotWorkingUnits().isEmpty();

    // if (hasUnavailable || hasNotWorking) {
    //     QStringList issues;

    //     if (hasUnavailable)
    //         issues << "Есть недоступные устройства:\n• " + this->healthForm->getNotAvailUnits().values().join("\n• ");
    //     if (hasNotWorking)
    //         issues << "Есть неработающие устройства:\n• " + this->healthForm->getNotWorkingUnits().values().join("\n• ");

    //     QMessageBox::critical(this, "Ошибка", "Невозможно начать печать!\n\n" + issues.join("\n\n"));
    //     return;
    // }

    ui->pb_start->setEnabled(false);
    ui->pb_stop->setEnabled(false);
    ui->cb_gtin_names->setEnabled(false);
    QUrl url = HttpManager::createApiUrl(QString("code-process/set-system-working/%1")
                                             .arg(ui->cb_gtin_names->getGtin()));
    httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){
        if (statusCode!=200 && statusCode!=-1) {
            // Преобразование ответа в JSON
            auto message = QString("");
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                message = jsonObj.value("msg").toString();
            }
            messagerInst.addMessage(QString("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: %1\nСообщение: %2")
                                        .arg(statusCode)
                                        .arg(message), Warning, false);
            QMessageBox::warning(this, "Ошибка", "Невозможно начать печать!\nНекоторые из устройств недоступны!");
            ui->pb_start->setEnabled(true);
            ui->pb_stop->setEnabled(true);
        } else if (statusCode==-1) {
            messagerInst.addMessage(QString("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: %1")
                                        .arg(statusCode), Error, true);
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
            // Преобразование ответа в JSON
            auto message = QString("");
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                message = jsonObj.value("msg").toString();
            }
            messagerInst.addMessage(QString("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: %1\nСообщение: %2")
                                        .arg(statusCode)
                                        .arg(message), Error, true);
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
        } else if (statusCode==-1) {
            messagerInst.addMessage(QString("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: %1")
                                        .arg(statusCode), Error, true);
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
        } else {
            ui->pb_stop->setEnabled(true);
            ui->pb_start->setEnabled(true);
            setSystemState(Stopped);
        }
    });
}

