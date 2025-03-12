#include "productsdatetimeedit.h"
#include "core/globalsettings.h"
#include "core/messager.h"
#include "qcoreevent.h"
#include <QEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QTextFormat>


ProductsCalendarWidget::ProductsCalendarWidget(QWidget *parent):
    QCalendarWidget(parent)
{
}

void ProductsCalendarWidget::setDatesToPaint(const QList<QDate> &newDatesToPaint)
{
    datesToPaint = newDatesToPaint;
}

void ProductsCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    // Вызов базового метода для отрисовки ячейки
    QCalendarWidget::paintCell(painter, rect, date);

    // Проверяем, есть ли текущая дата в списке datesToPaint
    for (const QDate& dt : datesToPaint) {
        if (date == dt) {
            if(selectedDate() == dt)
                break;
            // Настройка фона с прозрачностью
            QColor greenColor(0, 255, 0); // Зеленый цвет
            greenColor.setAlpha(100); // Прозрачность
            painter->fillRect(rect, greenColor); // Закрашиваем ячейку с прозрачным фоном

            // Устанавливаем цвет текста
            QColor textColor;
            if(gSettings.getTheme() == StyleManager::ThemeType::LightStyle){
                textColor = Qt::black;
            } else {
                textColor = QColor(255, 255, 255); // Белый цвет текста
            }
            painter->setPen(textColor); // Применяем цвет текста

            // Рисуем текст (дату) в ячейке
            QRect textRect = rect;
            textRect.moveTop(textRect.top());
            painter->drawText(textRect, Qt::AlignCenter, QString::number(date.day())); // Рисуем день
            break;
        }
    }
}

ProductsDateTimeEdit::ProductsDateTimeEdit(QWidget* parent):
    QDateTimeEdit(parent)
{
    m_httpManager = new HttpManager();
    setCalendarPopup(true);
    m_calendar = new ProductsCalendarWidget();
    m_calendar->installEventFilter(this);
    setCalendarWidget(m_calendar);
    setDateTime(QDateTime::currentDateTime());
    connect(m_calendar, &ProductsCalendarWidget::currentPageChanged, [this](const int year, const int month){
        QDate dt(year, month, 1);
        getAllDays(dt);
    });
}

ProductsDateTimeEdit::~ProductsDateTimeEdit()
{
    delete m_calendar;
    delete m_httpManager;
}

void ProductsDateTimeEdit::setGetGtinCallback(const std::function<QString ()> &newCallback)
{
    getGtinCallback = newCallback;
}

void ProductsDateTimeEdit::getAllDays(const QDate& dt)
{
    auto date = dt.toString("yyyy_MM");
    QUrl url = HttpManager::createApiUrl(QString("code-export/get-gtin-entry-dates/%1/%2")
                                             .arg(getGtin())
                                             .arg(date));
    // m_httpManager->makeRequest(url,
    //                            QJsonDocument(),
    //                            HttpManager::HttpMethod::Get,
    //                            std::bind(&ProductsDateTimeEdit::getAllDaysSlot, this, std::placeholders::_1, std::placeholders::_2));

    qDebug() << "GetAllDays";
    QNetworkReply* reply = m_httpManager->makeRequestAsync(url, QJsonDocument(), HttpManager::HttpMethod::Get);
    if (!reply) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-gtin-entry-dates!", Error, true);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-gtin-entry-dates!", Error, true);
            reply->deleteLater();
            return;
        }

        QByteArray responseData = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode != 200) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-gtin-entry-dates! Код ответа: "
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
        QList<QDate> dates;
        for (const QJsonValue &value : jsonArray) {
            QDate date = QDate::fromString(value.toString(), "yyyy_MM_dd");
            dates.append(date);
        }
        m_calendar->setDatesToPaint(dates);
        m_calendar->repaint();
        m_calendar->update();
        reply->deleteLater();
    });
}

void ProductsDateTimeEdit::setCurrentGtin(const QString &newCurrentGtin)
{
    currentGtin = newCurrentGtin;
}

bool ProductsDateTimeEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_calendar) {
        if (event->type() == QEvent::Show) {
            getAllDays(m_calendar->selectedDate());
        }
    }
    return QDateTimeEdit::eventFilter(watched, event);
}

void ProductsDateTimeEdit::getAllDaysSlot(const QByteArray &responseData, int statusCode)
{
    if (statusCode!=200 && statusCode!=-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-gtin-entry-dates! Код ответа: "
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
    } else if (statusCode==-1) {
        messagerInst.addMessage("Не удалось выполнить запрос api/v1/code-export/get-gtin-entry-dates! Код ответа: "
                                    +QString::number(statusCode)
                                    +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
    } else {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if(!jsonDoc.isArray()){
            qDebug() << "JSON is not an array!";
            return;
        }
        QJsonArray jsonArray = jsonDoc.array();
        QList<QDate> dates;
        for (const QJsonValue &value : jsonArray) {
            QDate date = QDate::fromString(value.toString(), "yyyy_MM_dd");
            dates.append(date);
        }
        m_calendar->setDatesToPaint(dates);
        m_calendar->repaint();
        m_calendar->update();
    }
}
