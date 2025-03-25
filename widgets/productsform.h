#ifndef PRODUCTSFORM_H
#define PRODUCTSFORM_H

#include <QWidget>

#include "core/httpmanager.h"
#include "models/productsremaindfiltermodel.h"
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

private slots:
    void on_pb_update_clicked();

private:
    Ui::ProductsForm *ui;

    ProductsRemaindModel* productsRemaindSourceModel;
    ProductsRemaindFilterModel* productsRemaindFilterModel;
    HttpManager* httpManager;
    void initModels();
    inline void initView();
    void selectProductsAsync();
    void fillTableWithProducts(const QJsonArray& responseData);
    QString error = "Не удалось выполнить запрос code-export/get-all-gtins-with-remainds";
};

#endif // PRODUCTSFORM_H
