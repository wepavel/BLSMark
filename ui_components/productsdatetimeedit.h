#ifndef PRODUCTSDATETIMEEDIT_H
#define PRODUCTSDATETIMEEDIT_H

#include "core/httpmanager.h"
#include <QDateTimeEdit>
#include <qcalendarwidget.h>

class ProductsCalendarWidget : public QCalendarWidget
{
public:
    ProductsCalendarWidget(QWidget* parent = nullptr);

    void setDatesToPaint(const QList<QDate> &newDatesToPaint);

private:
    QList<QDate> datesToPaint = {};

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;
};


class ProductsDateTimeEdit : public QDateTimeEdit
{
public:
    ProductsDateTimeEdit(QWidget* parent = nullptr);
    ~ProductsDateTimeEdit();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);

    void getAllDays(const QDate& dt);

    void setCurrentGtin(const QString &newCurrentGtin);

    void setGetGtinCallback(const std::function<QString ()> &newCallback);

    void setShowExportedCodes(bool newShowExportedCodes);

private:
    ProductsCalendarWidget* m_calendar;
    HttpManager* m_httpManager;
    QString currentGtin = "";
    bool showExportedCodes = false;

    std::function<QString()> getGtinCallback;
    QString getGtin(){return getGtinCallback();};

    void getAllDaysSlot(const QByteArray &responseData, int statusCode);
};

#endif // PRODUCTSDATETIMEEDIT_H
