#include "connectionstateform.h"
#include "qdatetime.h"
#include "qheaderview.h"
#include "ui_connectionstateform.h"
#include "widgets/statelabel.h"
#include <QTableWidget>
#include <qcheckbox.h>

#define ROW_COUNT 2
#define COLUMN_COUNT 4

ConnectionStateForm::ConnectionStateForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionStateForm)
{
    ui->setupUi(this);
    initStatesTable();
    initGoodsTable();
    initHealthChecker();
}

ConnectionStateForm::~ConnectionStateForm()
{
    delete mdl;
    delete h_checker;
}

void ConnectionStateForm::initStatesTable()
{
    // ПРОВЕРИТЬ НА УТЕЧКИ НО ВРОДЕ ВСЕ ОК
    // Добавляем в таблицу строки и столбцы
    for (int row = 0; row < ROW_COUNT; ++row) {
        for (int col = 0; col < COLUMN_COUNT; ++col) {
            // Создаем контейнер с макетом
            QWidget *container = new QWidget(ui->tw_states);
            QHBoxLayout *layout = new QHBoxLayout(container);
            layout->setAlignment(Qt::AlignCenter);  // Центрируем элементы в контейнере

            // Создаем StateLabel
            StateLabel *st_lbl = new StateLabel(container);
            lbls.append(st_lbl);
            layout->addWidget(st_lbl);  // Добавляем StateLabel в макет

            // Добавляем контейнер в ячейку таблицы
            ui->tw_states->setCellWidget(row, col, container);
        }
    }

    ui->tw_states->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tw_states->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tw_states->setGridStyle(Qt::SolidLine);  // Применяем сплошные линии
    ui->tw_states->setShowGrid(true);  // Включаем отображение сетки
    ui->tw_states->horizontalHeader()->setSectionsClickable(false);
    ui->tw_states->verticalHeader()->setSectionsClickable(false);

}

void ConnectionStateForm::initGoodsTable()
{
    mdl = new GoodsModel(ui->tv_goods);
    ui->tv_goods->setModel(mdl);
    ui->tv_goods->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tv_goods->horizontalHeader()->setStretchLastSection(true);
    ui->tv_goods->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tv_goods->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_goods->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    ui->tv_goods->setFocusPolicy(Qt::ClickFocus);

    qint64 currentTimeInSeconds = QDateTime::currentSecsSinceEpoch();

    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
}

void ConnectionStateForm::initHealthChecker()
{
    h_checker = new HealthChecker(this);
    connect(h_checker, &HealthChecker::serviceIsAvailable,this, [this](){
        lbls[0]->setState(true);
    });
    connect(h_checker, &HealthChecker::serviceIsNotAvailable,this, [this](){
        lbls[0]->setState(false);
    });
}


