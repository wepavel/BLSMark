#include "goodsdatetimeedit.h"
#include "core/globalsettings.h"
#include "core/messager.h"
#include "qcoreevent.h"
#include "qevent.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qpainter.h"
#include "qtextformat.h"


GoodsCalendarWidget::GoodsCalendarWidget(QWidget *parent):
    QCalendarWidget(parent)
{
}

void GoodsCalendarWidget::setDatesToPaint(const QList<QDate> &newDatesToPaint)
{
    datesToPaint = newDatesToPaint;
}

void GoodsCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
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

GoodsDateTimeEdit::GoodsDateTimeEdit(QWidget* parent):
    QDateTimeEdit(parent)
{
    m_httpManager = new HttpManager();
    setCalendarPopup(true);
    m_calendar = new GoodsCalendarWidget();
    m_calendar->installEventFilter(this);
    setCalendarWidget(m_calendar);
    setDateTime(QDateTime::currentDateTime());
    connect(m_calendar, &GoodsCalendarWidget::currentPageChanged, [this](const int year, const int month){
        QDate dt(year, month, 1);
        getAllDays(dt);
    });
}

GoodsDateTimeEdit::~GoodsDateTimeEdit()
{
    delete m_calendar;
    delete m_httpManager;
}

void GoodsDateTimeEdit::setGetGtinCallback(const std::function<QString ()> &newCallback)
{
    getGtinCallback = newCallback;
}

void GoodsDateTimeEdit::getAllDays(const QDate& dt)
{
    auto date = dt.toString("yyyy_MM");
    QUrl url = HttpManager::createApiUrl(QString("code-export/get-gtin-entry-dates/%1/%2")
                                             .arg(getGtin())
                                             .arg(date));
    m_httpManager->makeRequest(url,
                               QJsonDocument(),
                               HttpManager::HttpMethod::Get,
                               std::bind(&GoodsDateTimeEdit::getAllDaysSlot, this, std::placeholders::_1, std::placeholders::_2));
}

void GoodsDateTimeEdit::setCurrentGtin(const QString &newCurrentGtin)
{
    currentGtin = newCurrentGtin;
}

bool GoodsDateTimeEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_calendar) {
        if (event->type() == QEvent::Show) {
            getAllDays(m_calendar->selectedDate());
        }
    }
    return QDateTimeEdit::eventFilter(watched, event);
}

void GoodsDateTimeEdit::getAllDaysSlot(const QByteArray &responseData, int statusCode)
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
