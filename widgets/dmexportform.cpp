#include "dmexportform.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "ui_dmexportform.h"
#include <QCalendarWidget>
#include <QDateEdit>

DMExportForm::DMExportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMExportForm)
{
    ui->setupUi(this);
    ui->dte_date->setGetGtinCallback(std::bind(&GtinNamesComboBox::getGtin, ui->cb_goods));
    httpManager = new HttpManager(this);
    goodsMdl = new UnloadGoodsModel(this);
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
    httpManager->makeRequest(url,
                             QJsonDocument(),
                             HttpManager::HttpMethod::Get,
                             std::bind(&DMExportForm::fillGoodsTable, this, std::placeholders::_1, std::placeholders::_2));
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
        for (const QJsonValue &value : jsonArray) {
            auto obj = value.toObject();
            goodsMdl->addRow(obj["dm_code"].toString(), obj["product_name"].toString());
        }
    }
}

