#ifndef CONNECTIONSTATEFORM_H
#define CONNECTIONSTATEFORM_H

#include <QWidget>
#include "core/healthchecker.h"
#include "models/goodsmodel.h"
#include "widgets/statelabel.h"
#include "widgets/healthcheckform.h"
#include "widgets/statelabel.h"
#include <QTableWidget>
#include <qcheckbox.h>

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
    Ui::ConnectionStateForm *ui;
    void initGoodsTable();
    void initHealthChecker();

    GoodsModel* mdl;
    HealthChecker* m_hChecker;
    HealthCheckForm* m_healthCheckForm;
    QTableView* m_tvGoods;
};

#endif // CONNECTIONSTATEFORM_H
