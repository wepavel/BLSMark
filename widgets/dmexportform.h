#ifndef DMEXPORTFORM_H
#define DMEXPORTFORM_H

#include "core/httpmanager.h"
#include "models/exportproductsmodel.h"
#include <QWidget>

namespace Ui {
class DMExportForm;
}

class DMExportForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMExportForm(QWidget *parent = nullptr);
    ~DMExportForm();

private slots:
    void on_pb_search_clicked();

    void on_pb_load_in_csv_clicked();

    void on_chb_exported_stateChanged(int checked);

private:
    Ui::DMExportForm *ui;
    HttpManager* httpManager;
    ExportProductsModel* productModel;

    QString choosenDate="";
    QString choosenName="";

    void fillProductsTable(const QByteArray &responseData, int statusCode);
    void exportDmCodes(const QJsonArray& dmCodesArray);
    QVariant ObjectOrArrayFromString(const QString& in);
};

#endif // DMEXPORTFORM_H
