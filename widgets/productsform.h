#ifndef PRODUCTSFORM_H
#define PRODUCTSFORM_H

#include <QWidget>

#include "models/productsremaindmodel.h"

namespace Ui {
class ProductsForm;
}

class ProductsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductsForm(QWidget *parent = nullptr);
    ~ProductsForm();

private:
    Ui::ProductsForm *ui;
    //---Vars
    ProductsRemaindModel* productsRemaindModel;
};

#endif // PRODUCTSFORM_H
