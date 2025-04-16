#include <QDateTime>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>

#include "qdialog.h"
#include "qmessagebox.h"
#include "ui_connectionstateform.h"
#include "connectionstateform.h"
#include "ui_components/gtinnamescombobox.h"
#include "widgets/dminfoform.h"

ConnectionStateForm::ConnectionStateForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionStateForm)
{
    ui->setupUi(this);
    initHealthChecker();
    initControlPanel();
    initProductsTable();
    initConnections();
    m_tvProducts->verticalHeader()->setVisible(false); // убираем нумерацию строк
    setFocusPolicy(Qt::NoFocus);
}

ConnectionStateForm::~ConnectionStateForm()
{
    delete mdl;
    delete m_hChecker;
    delete m_tvProducts;
    delete m_healthCheckForm;
    delete m_dmCodeHandler;
}

void ConnectionStateForm::initProductsTable()
{
    m_tvProducts = new AutoScrollTableView(this);
    m_tvProducts->setFocusPolicy(Qt::NoFocus);

    layout()->addWidget(m_tvProducts);

    mdl = new ProductsModel(m_tvProducts);
    m_tvProducts->setModel(mdl);
    m_tvProducts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //m_tvProducts->horizontalHeader()->setStretchLastSection(true);
    m_tvProducts->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tvProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tvProducts->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    m_tvProducts->setFocusPolicy(Qt::NoFocus);

    connect(mdl, &ProductsModel::dataHasBeenAdded, m_tvProducts, &AutoScrollTableView::update);

    connect(controlPanelWidget->getClearHistoryPbPtr(), &QPushButton::clicked, [this](){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Подтверждение");
        msgBox.setText("Вы уверены, что хотите очистить коды из таблицы?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, "Да");
        msgBox.setButtonText(QMessageBox::No, "Нет");

        if (msgBox.exec() == QMessageBox::Yes) {
            this->mdl->clear();
        }
    });

    connect(m_tvProducts, &QTableView::doubleClicked, [this](const QModelIndex &index) {
        if(!index.isValid()){
            return;
        }
        int row = index.row();
        QModelIndex codeIdx = mdl->index(row, ProductsModel::CodeColumn);
        QString code = mdl->data(codeIdx, Qt::DisplayRole).toString();
        if(code.isEmpty())
            return;
        DMInfoForm::showInfoDialog(this, code);
    });
}

void ConnectionStateForm::initControlPanel()
{
    controlPanelWidget = new ControlPanelForm(this->m_healthCheckForm, this);
    layout()->addWidget(controlPanelWidget);
    // auto hrz_layout = new QHBoxLayout(this);
    // hrz_layout->addWidget(gtin_cb);
    // m_tvProducts = new AutoScrollTableView(this);
    // m_tvProducts->setFocusPolicy(Qt::NoFocus);

    // layout()->addWidget(m_tvProducts);

    // mdl = new ProductsModel(m_tvProducts);
    // m_tvProducts->setModel(mdl);
    // m_tvProducts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // //m_tvProducts->horizontalHeader()->setStretchLastSection(true);
    // m_tvProducts->setSelectionBehavior(QAbstractItemView::SelectRows);
    // m_tvProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    // m_tvProducts->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    // m_tvProducts->setFocusPolicy(Qt::NoFocus);

    // connect(mdl, &ProductsModel::dataHasBeenAdded, m_tvProducts, &AutoScrollTableView::update);
}

void ConnectionStateForm::initHealthChecker()
{
    m_healthCheckForm = new HealthCheckForm(this);
    layout()->addWidget(m_healthCheckForm);

    m_hChecker = new WsManager(this);
    m_dmCodeHandler = new DmCodeWsHandler(m_hChecker->getConnection());
}

void ConnectionStateForm::initConnections()
{
    connect(m_dmCodeHandler, &DmCodeWsHandler::dmCodeReceived, this, &ConnectionStateForm::dm_code_received);
    connect(m_hChecker, &WsManager::deviceAvailableChanged, this, &ConnectionStateForm::device_available_changed);
    connect(m_hChecker, &WsManager::deviceWorksChanged, this, &ConnectionStateForm::device_works_changed);
    connect(m_hChecker, &WsManager::sendApplicatorStateData, this->controlPanelWidget, &ControlPanelForm::processWsData);
    connect(m_hChecker, &WsManager::deviceWorksChanged, this->controlPanelWidget, &ControlPanelForm::serverWorksChanged);
    connect(m_hChecker, &WsManager::deviceAvailableChanged, this->controlPanelWidget, &ControlPanelForm::serverAvailableChanged);
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
        QString upload_date = messageObj.value("upload_time").toString();
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


