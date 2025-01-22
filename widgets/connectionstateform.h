#ifndef CONNECTIONSTATEFORM_H
#define CONNECTIONSTATEFORM_H

#include <QWidget>
#include "models/goodsmodel.h"

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

    GoodsModel* mdl;
};

#endif // CONNECTIONSTATEFORM_H
