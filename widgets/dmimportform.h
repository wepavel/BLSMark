#ifndef DMIMPORTFORM_H
#define DMIMPORTFORM_H

#include <QWidget>
#include <QProcess>
#include <QFutureWatcher>
#include <QProgressDialog>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

#include "core/globalsettings.h"
#include "models/dmimportmodel.h"
#include "crud/crudbase.h"

namespace Ui {
class DMImportForm;
}

class DMImportForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMImportForm(QWidget *parent = nullptr);
    ~DMImportForm();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        const QMimeData* mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            QList<QUrl> urlList = mimeData->urls();
            for (const QUrl &url : urlList) {
                if (url.isLocalFile()) {
                    qDebug() << url.toLocalFile();
                    // startReadDm(pdf_importer_path, url.toLocalFile())
                    // processDroppedFile(url.toLocalFile());
                }
            }
        }
        event->acceptProposedAction();
    }

private slots:
    void on_pb_load_file_clicked();
    void init_process();
    void recieve_dm_data(QString row);
    void complete_process();

private:
    //---Vars
    Ui::DMImportForm *ui;
    DMImportModel* importModel;
    QProgressDialog *progressDialog;
    QString pdf_importer_path = gSettings.get_app_path() + "/process/extract_datamatrix.exe";
    CRUDBase db = CRUDBase("QSQLITE", gSettings.get_app_path() + "/mydb.sqlite");

    //---Funcs
    void startReadDm(const QString &program, const QStringList &arguments);
    void setupImportTable();

};

#endif // DMIMPORTFORM_H
