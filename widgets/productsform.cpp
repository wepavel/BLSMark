#include "productsform.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "ui_productsform.h"
#include <qjsondocument.h>

ProductsForm::ProductsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProductsForm)
{
    ui->setupUi(this);
    initModels();
    initView();
    httpManager = new HttpManager(this);
    selectProductsAsync();
}

void ProductsForm::initModels()
{
    productsRemaindSourceModel = new ProductsRemaindModel(this);
    productsRemaindFilterModel = new ProductsRemaindFilterModel(this);
    productsRemaindFilterModel->setSourceModel(productsRemaindSourceModel);
    ui->tv_product_remaind->setModel(productsRemaindFilterModel);
    connect(ui->le_filter, &QLineEdit::textChanged, productsRemaindFilterModel, &ProductsRemaindFilterModel::setFilterProductName);
}

inline void ProductsForm::initView()
{
    ui->tv_product_remaind->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ProductsForm::selectProductsAsync()
{
    QUrl url = HttpManager::createApiUrl("code-export/get-all-gtins-with-remainds");
    ui->pb_update->setEnabled(false);
    QNetworkReply* reply = httpManager->makeRequestAsync(url, QJsonDocument(), HttpManager::HttpMethod::Get);
    if (!reply) {
        messagerInst.addMessage(error, Error, true);
        ui->pb_update->setEnabled(true);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            messagerInst.addMessage(error, Error, true);
            ui->pb_update->setEnabled(true);
            reply->deleteLater();
            return;
        }

        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 200) {
            messagerInst.addMessage(error
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            ui->pb_update->setEnabled(true);
            reply->deleteLater();
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(!jsonDoc.isArray()){
            qDebug() << "JSON is not an array!";
            ui->pb_update->setEnabled(true);
            return;
        }

        QJsonArray jsonArray = jsonDoc.array();
        this->fillTableWithProducts(jsonArray);
        ui->pb_update->setEnabled(true);
        reply->deleteLater();
    });
}

void ProductsForm::fillTableWithProducts(const QJsonArray &responseData)
{
    for (const QJsonValue &value : responseData) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            //QJsonObject({"code":"04654682048018","name":"very long tovar name","remainder":10})
            QString productName = obj["name"].toString();
            int remainder = obj["remainder"].toInt();
            productsRemaindSourceModel->addRow(productName, remainder);
        }
    }
}

ProductsForm::~ProductsForm()
{
    delete ui;
}

void ProductsForm::on_pb_update_clicked()
{
    productsRemaindSourceModel->clear();
    selectProductsAsync();
}

