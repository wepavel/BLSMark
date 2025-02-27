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
    ui->dte_date->setGetGtinCallback(std::bind(&GtinNamesComboBox::getGtin, ui->cb_goods));
    httpManager = new HttpManager(this);
    goodsMdl = new ExportGoodsModel(this);
    ui->tv_goods->setModel(goodsMdl);
    ui->tv_goods->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DMExportForm::~DMExportForm()
{
    delete ui;
    delete httpManager;
    delete goodsMdl;
}

void DMExportForm::on_pb_search_clicked()
{
    goodsMdl->clear();
    QUrl url = HttpManager::createApiUrl(QString("code-export/get-gtin-dmcodes-by-date/%1/%2")
                                             .arg(ui->cb_goods->getGtin())
                                             .arg(ui->dte_date->date().toString("yyyy_MM_dd")));
    ui->pb_search->setEnabled(false);
    httpManager->makeRequest(url,
                             QJsonDocument(),
                             HttpManager::HttpMethod::Get,
                             std::bind(&DMExportForm::fillGoodsTable, this, std::placeholders::_1, std::placeholders::_2));
    ui->pb_search->setEnabled(true);
}

void DMExportForm::fillGoodsTable(const QByteArray &responseData, int statusCode)
{
    if (statusCode!=200 && statusCode!=-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins! Код ответа: "
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
    } else if (statusCode==-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins! Код ответа: "
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
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
            goodsMdl->addRow(obj["dm_code"].toString(), obj["product_name"].toString());
        }
        this->choosenName=ui->cb_goods->currentText();
        this->choosenDate=ui->dte_date->date().toString("yyyy_MM_dd");
    }
}


void DMExportForm::on_pb_load_in_csv_clicked()
{
    if(goodsMdl->isEmpty()){
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

    auto res = goodsMdl->saveToCsv(filePath);

    if (res.first) {
        QMessageBox::information(this, tr("Успех"), tr("Файл успешно сохранен в: ") + filePath);
    } else {
        QMessageBox::critical(this, tr("Ошибка"), res.second);
    }
}

