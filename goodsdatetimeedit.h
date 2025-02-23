#ifndef GOODSDATETIMEEDIT_H
#define GOODSDATETIMEEDIT_H

#include "core/httpmanager.h"
#include <QDateTimeEdit>
#include <qcalendarwidget.h>

class GoodsCalendarWidget : public QCalendarWidget
{
public:
    GoodsCalendarWidget(QWidget* parent = nullptr);

    void setDatesToPaint(const QList<QDate> &newDatesToPaint);

private:
    QList<QDate> datesToPaint = {};

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;
};


class GoodsDateTimeEdit : public QDateTimeEdit
{
public:
    GoodsDateTimeEdit(QWidget* parent = nullptr);
    ~GoodsDateTimeEdit();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);

    void getAllDays(const QDate& dt);

    void setCurrentGtin(const QString &newCurrentGtin);

    void setGetGtinCallback(const std::function<QString ()> &newCallback);

private:
    GoodsCalendarWidget* m_calendar;
    HttpManager* m_httpManager;
    QString currentGtin = "";

    std::function<QString()> getGtinCallback;
    QString getGtin(){return getGtinCallback();};

    void getAllDaysSlot(const QByteArray &responseData, int statusCode);
};

#endif // GOODSDATETIMEEDIT_H
