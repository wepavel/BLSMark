#include <QDateTime>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>

#include "ui_connectionstateform.h"
#include "connectionstateform.h"
#include "ui_components/gtinnamescombobox.h"

ConnectionStateForm::ConnectionStateForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionStateForm)
{
    ui->setupUi(this);
    initHealthChecker();
    initControlPanel();
    initGoodsTable();
    m_tvGoods->verticalHeader()->setVisible(false); // убираем нумерацию строк
    setFocusPolicy(Qt::NoFocus);
}

ConnectionStateForm::~ConnectionStateForm()
{

    delete mdl;
    delete m_hChecker;
    delete m_tvGoods;
    delete m_healthCheckForm;
    delete m_dmCodeHandler;
}

void ConnectionStateForm::initGoodsTable()
{
    m_tvGoods = new AutoScrollTableView(this);
    m_tvGoods->setFocusPolicy(Qt::NoFocus);

    layout()->addWidget(m_tvGoods);

    mdl = new GoodsModel(m_tvGoods);
    m_tvGoods->setModel(mdl);
    m_tvGoods->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //m_tvGoods->horizontalHeader()->setStretchLastSection(true);
    m_tvGoods->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tvGoods->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tvGoods->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    m_tvGoods->setFocusPolicy(Qt::NoFocus);

    connect(mdl, &GoodsModel::dataHasBeenAdded, m_tvGoods, &AutoScrollTableView::update);
}

void ConnectionStateForm::initControlPanel()
{
    auto gtin_cb = new GtinNamesComboBox(this);
    gtin_cb->setFocusPolicy(Qt::NoFocus);
    // auto hrz_layout = new QHBoxLayout(this);
    // hrz_layout->addWidget(gtin_cb);
    layout()->addWidget(gtin_cb);
    // m_tvGoods = new AutoScrollTableView(this);
    // m_tvGoods->setFocusPolicy(Qt::NoFocus);

    // layout()->addWidget(m_tvGoods);

    // mdl = new GoodsModel(m_tvGoods);
    // m_tvGoods->setModel(mdl);
    // m_tvGoods->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // //m_tvGoods->horizontalHeader()->setStretchLastSection(true);
    // m_tvGoods->setSelectionBehavior(QAbstractItemView::SelectRows);
    // m_tvGoods->setSelectionMode(QAbstractItemView::SingleSelection);
    // m_tvGoods->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    // m_tvGoods->setFocusPolicy(Qt::NoFocus);

    // connect(mdl, &GoodsModel::dataHasBeenAdded, m_tvGoods, &AutoScrollTableView::update);
}

void ConnectionStateForm::initHealthChecker()
{
    m_healthCheckForm = new HealthCheckForm(this);
    layout()->addWidget(m_healthCheckForm);

    m_hChecker = new HealthChecker(this);
    m_dmCodeHandler = new DmCodeWsHandler(m_hChecker->getConnection());
    connect(m_dmCodeHandler, &DmCodeWsHandler::dmCodeReceived, this, &ConnectionStateForm::dm_code_received);

    connect(m_hChecker, &HealthChecker::deviceAvailableChanged, this, &ConnectionStateForm::device_available_changed);
    connect(m_hChecker, &HealthChecker::deviceWorksChanged, this, &ConnectionStateForm::device_works_changed);
}

QDateTime ConnectionStateForm::getDateTime(QString dtStr) const
{
    // dtStr.replace("_", " ");  // Заменяем "_" наdtStr
    //dtStrрока должна выглядеть как "2025 02 01 12 11 09"
    //dtStrправильный формат "yyyy-MM-dd HH:mm:ss"
    // dtStr.remove(" ");
    // dtStr.insert(4, ":");
    // dtStr.insert(7, ":");
    // dtStr.insert(10, " ");
    // dtStr.insert(13, ":");
    // dtStr.insert(16, ":");
    // Преобразуем строку в объект QDateTime
    // QDateTime dateTime = QDateTime::fromString(dtStr, "yyyy:MM:dd HH:mm:ss");
    QDateTime dateTime = QDateTime::fromString(dtStr, "yyyy_MM_dd_HHmmss");

    // Проверяем, если строка не валидна
    if (!dateTime.isValid())
        return QDateTime();
    return dateTime;
}

void ConnectionStateForm::device_available_changed(QString devName, bool available)
{
    m_healthCheckForm->setAvailable(devName, available);
}

void ConnectionStateForm::device_works_changed(QString devName, bool works)
{
    m_healthCheckForm->setWorks(devName, works);
}

void ConnectionStateForm::dm_code_received(const QString &msg)
{
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());

    if (!doc.isNull()) {
        // Получаем корневой объект
        QJsonObject rootObj = doc.object();

        // Извлекаем значения из объектов
        QString event = rootObj.value("event").toString();
        QJsonObject dataObj = rootObj.value("data").toObject();
        QString user_id = dataObj.value("user_id").toString();
        QJsonObject messageObj = dataObj.value("message").toObject();

        // Извлекаем значения из объекта message
        QString dm_code = messageObj.value("dm_code").toString();
        QString gtin = messageObj.value("gtin").toString();
        //QString serial_number = messageObj.value("serial_number").toString();
        QString country = messageObj.value("country").toString();
        QString product_name = messageObj.value("product_name").toString();

        //bool is_long_format = messageObj.value("is_long_format").toBool();
        //QString verification_key = messageObj.value("verification_key").toString();
        //QString verification_key_value = messageObj.value("verification_key_value").toString();
        QString upload_date = messageObj.value("upload_date").toString();
        QString entry_time = messageObj.value("entry_time").toString();
        //QString export_time = messageObj.value("export_time").toString();

        // qDebug() << "Event: " << event;
        // qDebug() << "User ID: " << user_id;
        // qDebug() << "DM Code: " << dm_code;
        // qDebug() << "GTIN: " << gtin;
        // //qDebug() << "Serial Number: " << serial_number;
        // qDebug() << "Country: " << country;
        // //qDebug() << "Is Long Format: " << is_long_format;
        // //qDebug() << "Verification Key: " << verification_key;
        // //qDebug() << "Verification Key Value: " << verification_key_value;
        // qDebug() << "Upload Date: " << upload_date;
        // qDebug() << "Entry Time: " << entry_time;
        // //qDebug() << "Export Time: " << export_time;

        mdl->addRow(dm_code,
                    product_name,
                    country,
                    getDateTime(upload_date).toSecsSinceEpoch(),
                    getDateTime(entry_time).toSecsSinceEpoch()
                    );
    }
}


