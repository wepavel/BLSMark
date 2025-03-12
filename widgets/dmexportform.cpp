#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QFileDialog>
#include <QMessageBox>
#include "dmexportform.h"
#include "core/messager.h"
#include "ui_dmexportform.h"

DMExportForm::DMExportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMExportForm)
{
    ui->setupUi(this);
    ui->dte_date->setGetGtinCallback(std::bind(&GtinNamesComboBox::getGtin, ui->cb_products));
    httpManager = new HttpManager(this);
    productModel = new ExportProductsModel(this);
    ui->tv_products->setModel(productModel);
    ui->tv_products->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DMExportForm::~DMExportForm()
{
    delete ui;
    delete httpManager;
    delete productModel;
}

void DMExportForm::on_pb_search_clicked()
{
    productModel->clear();
    QUrl url = HttpManager::createApiUrl(QString("code-export/get-gtin-dmcodes-by-date/%1/%2")
                                             .arg(ui->cb_products->getGtin())
                                             .arg(ui->dte_date->date().toString("yyyy_MM_dd")));
    ui->pb_search->setEnabled(false);
    httpManager->makeRequest(url,
                             QJsonDocument(),
                             HttpManager::HttpMethod::Get,
                             std::bind(&DMExportForm::fillProductsTable, this, std::placeholders::_1, std::placeholders::_2));
    ui->pb_search->setEnabled(true);
}

void DMExportForm::fillProductsTable(const QByteArray &responseData, int statusCode)
{
    if (statusCode!=200 && statusCode!=-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins! Код ответа: "
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
    } else if (statusCode==-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins! Код ответа: "
                                    +QString::number(statusCode), Error, true);
    } else {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(!jsonDoc.isArray()){
            qDebug() << "JSON is not an array!";
            return;
        }
        QJsonArray jsonArray = jsonDoc.array();
        if(jsonArray.isEmpty()){
            QMessageBox::warning(this, tr("Внимание"), tr("Товары за указанную дату не найдены!"));
            return;
        }
        for (const QJsonValue &value : jsonArray) {
            auto obj = value.toObject();
            productModel->addRow(obj["dm_code"].toString(), obj["product_name"].toString());
        }
        this->choosenName=ui->cb_products->currentText();
        this->choosenDate=ui->dte_date->date().toString("yyyy_MM_dd");
    }
}


void DMExportForm::on_pb_load_in_csv_clicked()
{
    if(productModel->isEmpty()){
        QMessageBox::warning(this, tr("Внимание"), tr("Отсутсвуют данные для выгрузки в таблице!"));
        return;
    }
    // Открываем диалог выбора папки
    QString fileName = choosenDate+"_"+choosenName.trimmed().replace(" ","_")+".csv";
    QString directory = QFileDialog::getExistingDirectory(this, tr("Выберите папку для сохранения файла ")+fileName,
                                                          QDir::homePath());

    // Проверяем, выбрал ли пользователь папку
    if (directory.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не выбрана папка для сохранения!"));
        return;
    }

    QString filePath = directory + "/" + fileName;

    auto res = productModel->saveToCsv(filePath);

    if (res.first) {
        QMessageBox::information(this, tr("Успех"), tr("Файл ") + filePath + tr(" успешно сохранен."));
    } else {
        QMessageBox::critical(this, tr("Ошибка"), res.second);
    }
}

