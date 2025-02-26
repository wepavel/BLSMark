#ifndef DMEXPORTFORM_H
#define DMEXPORTFORM_H

#include "core/httpmanager.h"
#include "models/exportgoodsmodel.h"
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

private:
    Ui::DMExportForm *ui;
    HttpManager* httpManager;
    UnloadGoodsModel* goodsMdl;

    QString choosenDate="";
    QString choosenName="";

    void fillGoodsTable(const QByteArray &responseData, int statusCode);
};

#endif // DMEXPORTFORM_H
