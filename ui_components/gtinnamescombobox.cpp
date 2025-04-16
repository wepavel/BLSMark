#include "gtinnamescombobox.h"
#include "core/messager.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qurl.h"

GtinNamesComboBox::GtinNamesComboBox(QWidget* parent):
    QComboBox(parent){
    m_httpManager = new HttpManager(this);
    updateNames();
}

GtinNamesComboBox::~GtinNamesComboBox()
{
    delete m_httpManager;
}

void GtinNamesComboBox::updateNames()
{
    QUrl url = HttpManager::createApiUrl("code-export/get-all-gtins");
    // httpManager->makeRequest(url,
    //                          QJsonDocument(),
    //                          HttpManager::HttpMethod::Get,
    //                          std::bind(&GtinNamesComboBox::getAllGtinsNamesSlot, this, std::placeholders::_1, std::placeholders::_2));

    QNetworkReply* reply = m_httpManager->makeRequestAsync(url, QJsonDocument(), HttpManager::HttpMethod::Get);
    if (!reply) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins!", Error, true);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins!", Error, true);
            reply->deleteLater();
            return;
        }

        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 200) {
            messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-all-gtins! Код ответа: "
                                        +QString::number(statusCode)
                                        +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
            reply->deleteLater();
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(!jsonDoc.isArray()){
            qDebug() << "JSON is not an array!";
            return;
        }

        QJsonArray jsonArray = jsonDoc.array();

        QString curCbText = currentText();
        // нам надо посмотреть будет ли в ответе текущее имя из комбобокса
        // если оно есть в ответе то мы его потом поставим обратно в комбобокс
        // если его нет в ответе то мы поставим текущим последнее имя товара из ответа
        bool textWasFoundInResponse = false;

        clear();

        // Проходим по каждому элементу массива
        for (const QJsonValue &value : jsonArray) {
            if (value.isObject()) {
                QJsonObject obj = value.toObject();

                // Извлекаем значения полей "name" и "code"
                QString name = obj.value("name").toString();
                if (name == curCbText){
                    textWasFoundInResponse = true;
                }
                QString code = obj.value("code").toString();
                setProperty(name.toUtf8(), code);
                addItem(name);
                // Выводим значения
                //qDebug() << "Name:" << name << ", GTIN (code):" << code;
            }
        }

        if(textWasFoundInResponse){
            setCurrentText(curCbText);
        }

        reply->deleteLater();
    });
}

QString GtinNamesComboBox::getGtin() const
{
    auto p = property(currentText().toUtf8()).toString();
    return p;
}

void GtinNamesComboBox::getAllGtinsNamesSlot(const QByteArray &responseData, int statusCode)
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

        QString curCbText = currentText();
        // нам надо посмотреть будет ли в ответе текущее имя из комбобокса
        // если оно есть в ответе то мы его потом поставим обратно в комбобокс
        // если его нет в ответе то мы поставим текущим последнее имя товара из ответа
        bool textWasFoundInResponse = false;

        clear();

        // Проходим по каждому элементу массива
        for (const QJsonValue &value : jsonArray) {
            if (value.isObject()) {
                QJsonObject obj = value.toObject();

                // Извлекаем значения полей "name" и "code"
                QString name = obj.value("name").toString();
                if (name == curCbText){
                    textWasFoundInResponse = true;
                }
                QString code = obj.value("code").toString();
                setProperty(name.toUtf8(), code);
                addItem(name);
                // Выводим значения
                qDebug() << "Name:" << name << ", GTIN (code):" << code;
            }
        }

        if(textWasFoundInResponse){
            setCurrentText(curCbText);
        }
    }
}

void GtinNamesComboBox::mousePressEvent(QMouseEvent *event)
{
    updateNames();
    QComboBox::mousePressEvent(event);
}
