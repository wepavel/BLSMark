#ifndef CONNECTIONSTATEFORM_H
#define CONNECTIONSTATEFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>

#include "core/dmcodewshandler.h"
#include "core/wsmanager.h"
#include "models/productsmodel.h"
#include "tables/autoscrolltableview.h"
#include "widgets/statelabel.h"
#include "widgets/healthcheckform.h"
#include "widgets/statelabel.h"
#include "controlpanelform.h"


namespace Ui {
class ConnectionStateForm;
}

class ConnectionStateForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionStateForm(QWidget *parent = nullptr);
    ~ConnectionStateForm();

private:
    //---Funcs
    Ui::ConnectionStateForm *ui;
    void initProductsTable();
    void initControlPanel();
    void initHealthChecker();
    void initConnections();
    QDateTime getDateTime(QString dtStr) const;

    //---Vars
    ProductsModel* mdl;
    WsManager* m_hChecker;
    HealthCheckForm* m_healthCheckForm;
    AutoScrollTableView* m_tvProducts;
    DmCodeWsHandler* m_dmCodeHandler;
    ControlPanelForm* controlPanelWidget;

private slots:
    void device_available_changed(QString devName, bool available);
    void device_works_changed(QString devName, bool works);
    void dm_code_received(const QString& msg);
};

#endif // CONNECTIONSTATEFORM_H
