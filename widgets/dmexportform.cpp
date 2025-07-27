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
#include "dminfoform.h"
#include "qevent.h"
#include "ui_dmexportform.h"

DMExportForm::DMExportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMExportForm)
{
    ui->setupUi(this);
    ui->dte_date->setGetGtinCallback(std::bind(&GtinNamesComboBox::getGtin, ui->cb_products));
    httpManager = new HttpManager(this);
    initProductTable();
    // productModel->addRow("code", "name");
    // productModel->addRow("code1", "name1");
    // productModel->addRow("code2", "name2");
    // productModel->addRow("code3", "name3");
    // productModel->addRow("code4", "name4");

    ui->pb_load_in_csv->setVisible(false);
    ui->pb_load_in_xlsx->setVisible(false);
}

DMExportForm::~DMExportForm()
{
    delete ui;
    delete httpManager;
    delete productModel;
}

void DMExportForm::on_pb_search_clicked()
{
    if (ui->cb_products->currentText().isEmpty()) {
        QMessageBox::warning(this,
                             "Внимание",
                             "Позиция на экспорт не выбрана, пожалуйста, выберите позицию",
                             QMessageBox::Ok);
        return;
    }

    QUrl url = HttpManager::createApiUrl(QString("code-export/get-gtin-dmcodes-by-date/%1/%2/%3")
                                             .arg(ui->chb_exported->isChecked())
                                             .arg(ui->cb_products->getGtin())
                                             .arg(ui->dte_date->date().toString("yyyy_MM_dd")));

    ui->pb_search->setEnabled(false);
    httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Get,
                             [&](const QByteArray& responseData, int statusCode) {
                                 if (statusCode == 200) {
                                     fillProductsTable(responseData);
                                 } else {
                                     auto message = QString("");
                                     QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
                                     if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                                         QJsonObject jsonObj = jsonDoc.object();
                                         message = jsonObj.value("msg").toString();
                                     }
                                     messagerInst.addMessage(QString("Не удалось выполнить запрос code-export/get-gtin-dmcodes-by-date/! Код ответа: %1\nСообщение: %2")
                                                                 .arg(statusCode)
                                                                 .arg(message), Error, true);
                                 }
                                 ui->pb_search->setEnabled(true);
                             });
}

void DMExportForm::fillProductsTable(const QByteArray &responseData)
{
    productModel->clear();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isArray()) {
        qDebug() << "JSON is not an array!";
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    if (jsonArray.isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Товары за указанную дату не найдены!"));
        return;
    }

    for (const QJsonValue &value : jsonArray) {
        auto obj = value.toObject();
        productModel->addRow(obj["dm_code"].toString(), obj["product_name"].toString());
    }

    this->choosenName = ui->cb_products->currentText();
    this->choosenDate = ui->dte_date->date().toString("yyyy_MM_dd");
}

QVariant DMExportForm::ObjectOrArrayFromString(const QString& in)
{
    QVariant result;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if (!doc.isNull())
    {
        if (doc.isObject()) {
            result = doc.object();
        } else if (doc.isArray()) {
            result = doc.array();
        } else {
            qDebug() << "Document is neither an object nor an array";
        }
    } else {
        qDebug() << "Invalid JSON...\n" << in;
    }

    return result;
}

void DMExportForm::initProductTable()
{
    productModel = new ExportProductsModel(this);
    ui->tv_products->setModel(productModel);
    ui->tv_products->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->tv_products, &QTableView::doubleClicked, [this](const QModelIndex &index) {
        if(!index.isValid()){
            return;
        }
        int row = index.row();
        QModelIndex codeIdx = productModel->index(row, ExportProductsModel::CodeColumn);
        QString code = productModel->data(codeIdx, Qt::DisplayRole).toString();
        if(code.isEmpty())
            return;
        DMInfoForm::showInfoDialog(this, code);
    });
}

void DMExportForm::exportDmCodes(const QJsonArray& dmCodesArray)
{
    QUrl url = HttpManager::createApiUrl("code-export/export_dmcodes");

    QNetworkReply* reply = httpManager->makeRequestAsync(url,
                                                         QJsonDocument(dmCodesArray),
                                                         HttpManager::HttpMethod::Post);
    if (!reply) {
        messagerInst.addMessage("Не удалось выполнить get запрос code-export/export_dmcodes", Error, true);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            messagerInst.addMessage("Не удалось выполнить get запрос code-export/export_dmcodes", Error, true);
            reply->deleteLater();
            return;
        }

        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 200) {
            messagerInst.addMessage("Не удалось выполнить get запрос code-export/export_dmcodes"
                                        +QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            reply->deleteLater();
            return;
        }

        QJsonArray result = ObjectOrArrayFromString(responseData).toJsonArray();

        if (result.isEmpty())
        {
            // QMessageBox::information(this, tr("Успешный экспорт DM-кодов"), tr("DM-коды были успешно экспортированы!"));
            messagerInst.addMessage(tr("Успешный экспорт DM-кодов. DM-коды были успешно экспортированы!"), Info);
        } else {
            for (const QJsonValue &value : qAsConst(result)) {
                QJsonObject jsonObject = value.toObject();
                QString message = QString("StatusCode: %1; Ошибка экспорта кода %2! %3")
                                      .arg(statusCode)
                                      .arg(jsonObject.value("dm_code").toString())
                                      .arg(jsonObject.value("problem").toString());
                messagerInst.addMessage(message, Warning, true);
            }
            QMessageBox::warning(this, tr("Внимание!"), tr("Не все коды были успешно эскпортированы!\nОбратите внимание на окно предупреждений!"));
        }
    });
}


void DMExportForm::on_pb_load_in_csv_clicked()
{
    auto res = saveFile("csv", [this](const QString &filePath) {
        return productModel->saveToCsv(filePath);
    });

    if (res.first) {
        exportDmCodes(productModel->getDmCodesArray());
    } else {
        QMessageBox::critical(this, tr("Ошибка"), res.second);
    }
}

void DMExportForm::on_chb_exported_stateChanged(int checked)
{
    ui->dte_date->setShowExportedCodes(checked);
}


void DMExportForm::on_pb_load_in_xlsx_clicked()
{
    auto res = saveFile("xlsx", [this](const QString &filePath) {
        return productModel->saveToXlsx(filePath);
    });

    if (res.first) {
        exportDmCodes(productModel->getDmCodesArray());
    } else {
        QMessageBox::critical(this, tr("Ошибка"), res.second);
    }
}

void DMExportForm::on_pb_load_in_xlsx_csv_clicked()
{
    // Проверка наличия данных для выгрузки
    if (productModel->isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Отсутсвуют данные для выгрузки в таблице!"));
        return;
    }

    QJsonArray jsonArray = productModel->getDmCodesArray();

    // Формируем базовое имя файла
    QString baseFileName = choosenDate + "_" + choosenName.trimmed().replace(" ", "_");

    // Открываем диалог выбора папки один раз
    QString directory = QFileDialog::getExistingDirectory(this, tr("Выберите папку для сохранения файлов ") + baseFileName, QDir::homePath());
    if (directory.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не выбрана папка для сохранения!"));
        return;
    }

    // Формирование полных путей для обоих форматов
    QString filePathXlsx = directory + "/" + baseFileName + ".xlsx";
    QString filePathCsv  = directory + "/" + baseFileName + ".csv";

    // Сохраняем данные в XLSX
    auto res_xlsx = productModel->saveToXlsx(filePathXlsx);
    if (!res_xlsx.first) {
        QMessageBox::critical(this, tr("Ошибка сохранения XLSX"), res_xlsx.second);
        return;
    }

    // Сохраняем данные в CSV
    auto res_csv = productModel->saveToCsv(filePathCsv);
    if (!res_csv.first) {
        QMessageBox::critical(this, tr("Ошибка сохранения CSV"), res_csv.second);
        return;
    }

    // Если оба файла сохранены успешно, вызываем общую функцию экспорта
    exportDmCodes(jsonArray);

    QString fileNameXlsx = QFileInfo(filePathXlsx).fileName();
    QString fileNameCsv  = QFileInfo(filePathCsv).fileName();
    QString message = tr("Файлы успешно сохранены:\n%1,\n%2").arg(fileNameXlsx).arg(fileNameCsv);
    QMessageBox::information(this, tr("Успех"), message);
}

QPair<bool, QString> DMExportForm::saveFile(const QString &extension, const std::function<QPair<bool, QString>(const QString&)> &saveFunction)
{
    if (productModel->isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Отсутсвуют данные для выгрузки в таблице!"));
        return QPair<bool, QString>(false, tr("Отсутсвуют данные для выгрузки"));
    }

    // Открываем диалог выбора папки
    QString fileName = choosenDate + "_" + choosenName.trimmed().replace(" ", "_") + "." + extension;
    QString directory = QFileDialog::getExistingDirectory(this, tr("Выберите папку для сохранения файла ") + fileName, QDir::homePath());

    // Проверяем, выбрал ли пользователь папку
    if (directory.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не выбрана папка для сохранения!"));
        return QPair<bool, QString>(false, tr("Не выбрана папка для сохранения"));
    }

    QString filePath = directory + "/" + fileName;

    // Вызываем соответствующую функцию для сохранения
    return saveFunction(filePath);
}


QPair<bool, QString> DMExportForm::saveFiles(const QString &extension, const std::function<QPair<bool, QString>(const QString&)> &saveFunction)
{
    if (productModel->isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Отсутсвуют данные для выгрузки в таблице!"));
        return QPair<bool, QString>(false, tr("Отсутсвуют данные для выгрузки"));
    }

    // Открываем диалог выбора папки
    QString fileName = choosenDate + "_" + choosenName.trimmed().replace(" ", "_") + "." + extension;
    QString directory = QFileDialog::getExistingDirectory(this, tr("Выберите папку для сохранения файла ") + fileName, QDir::homePath());

    // Проверяем, выбрал ли пользователь папку
    if (directory.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не выбрана папка для сохранения!"));
        return QPair<bool, QString>(false, tr("Не выбрана папка для сохранения"));
    }

    QString filePath = directory + "/" + fileName;

    // Вызываем соответствующую функцию для сохранения
    return saveFunction(filePath);
}


