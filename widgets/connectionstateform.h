#ifndef CONNECTIONSTATEFORM_H
#define CONNECTIONSTATEFORM_H

#include <QWidget>
#include "core/healthchecker.h"
#include "models/goodsmodel.h"
#include "widgets/statelabel.h"

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
    void initStatesTable();
    void initGoodsTable();
    void initHealthChecker();

    QVector<StateLabel*> lbls;
    GoodsModel* mdl;
    HealthChecker* h_checker;
};

#endif // CONNECTIONSTATEFORM_H
