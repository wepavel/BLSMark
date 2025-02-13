#include "dmexportform.h"
#include "ui_dmexportform.h"

#include <QCalendarWidget>
#include <QDateEdit>

DMExportForm::DMExportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMExportForm)
{
    ui->setupUi(this);
}

DMExportForm::~DMExportForm()
{
    delete ui;
}

void DMExportForm::on_pb_calendar_clicked()
{
    // Создаем календарь
    QCalendarWidget *calendar = new QCalendarWidget(this);
    calendar->setWindowModality(Qt::ApplicationModal);
    calendar->setWindowFlags(Qt::Popup);

    // Убираем нумерацию недель
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);


    // Устанавливаем позицию календаря относительно кнопки
    QPoint pos = ui->pb_calendar->mapToGlobal(QPoint(0, ui->pb_calendar->height()));
    calendar->move(pos);

    // Подключаем сигнал выбора даты
    connect(calendar, &QCalendarWidget::clicked, this, [=](const QDate &date) {
        // Предполагаем, что у вас есть QDateEdit с именем dateEdit
        // Если нет, замените на соответствующий виджет или обработку
        // ui->dateEdit->setDate(date);
        calendar->close();
        calendar->deleteLater();
    });

    // Показываем календарь
    calendar->show();
}

