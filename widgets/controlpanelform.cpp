#include "controlpanelform.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
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
    if(product.isEmpty()){
        ui->lbl_currentProduct->setText("пусто");
        ui->lbl_currentProduct->setStyleSheet("color: red;");
        return;
    }
    ui->lbl_currentProduct->setText(product);
    ui->lbl_currentProduct->setStyleSheet("color: green;");
}

void ControlPanelForm::updateState()
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

void ControlPanelForm::on_pb_start_stop_clicked()
{
    ui->pb_start_stop->setEnabled(false);
    QUrl url = HttpManager::createApiUrl(QString("code-process/set-system-working/%1")
                                             .arg(ui->cb_gtin_names->property("gtin_code").toString()));
    httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){
        if (statusCode!=200 && statusCode!=-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_start_stop->setEnabled(true);
        } else if (statusCode==-1) {
            messagerInst.addMessage("Не удалось выполнить запрос code-process/set-system-working/! Код ответа: "+QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_start_stop->setEnabled(true);
        } else {
            ui->pb_start_stop->setEnabled(true);
            updateState();
        }
    });
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

