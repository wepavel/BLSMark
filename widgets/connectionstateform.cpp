#include "connectionstateform.h"
#include "qdatetime.h"
#include "qheaderview.h"
#include "ui_connectionstateform.h"


#define ROW_COUNT 2
#define COLUMN_COUNT 4

ConnectionStateForm::ConnectionStateForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionStateForm)
{
    ui->setupUi(this);
    initHealthChecker();
    initGoodsTable();
}

ConnectionStateForm::~ConnectionStateForm()
{
    delete mdl;
    delete m_hChecker;
    delete m_tvGoods;
    delete m_healthCheckForm;
}

void ConnectionStateForm::initGoodsTable()
{
    m_tvGoods = new QTableView(this);
    layout()->addWidget(m_tvGoods);

    mdl = new GoodsModel(m_tvGoods);
    m_tvGoods->setModel(mdl);
    m_tvGoods->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tvGoods->horizontalHeader()->setStretchLastSection(true);
    m_tvGoods->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tvGoods->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tvGoods->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    m_tvGoods->setFocusPolicy(Qt::ClickFocus);

    qint64 currentTimeInSeconds = QDateTime::currentSecsSinceEpoch();

    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
    mdl->addRow("21321", "Товар", "РОССИЯ", currentTimeInSeconds, currentTimeInSeconds-100);
}

void ConnectionStateForm::initHealthChecker()
{
    m_healthCheckForm = new HealthCheckForm(this);
    layout()->addWidget(m_healthCheckForm);

    m_hChecker = new HealthChecker(this);

    connect(m_hChecker, &HealthChecker::deviceAvailableChanged, this, &ConnectionStateForm::device_available_changed);
    connect(m_hChecker, &HealthChecker::deviceWorksChanged, this, &ConnectionStateForm::device_works_changed);
}

void ConnectionStateForm::device_available_changed(QString devName, bool available)
{
    m_healthCheckForm->setAvailable(devName, available);
}

void ConnectionStateForm::device_works_changed(QString devName, bool works)
{
    m_healthCheckForm->setWorks(devName, works);
}


