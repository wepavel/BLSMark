#ifndef DMIMPORTFORM_H
#define DMIMPORTFORM_H

#include <QWidget>
#include <QProcess>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

#include "core/globalsettings.h"
#include "models/dmimportmodel.h"
#include "crud/cruddmcode.h"
#include "dialogs/doubleprogressdialog.h"

namespace Ui {
class DMImportForm;
}

class DMImportForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMImportForm(QWidget *parent = nullptr);
    ~DMImportForm();

    void saveImage(const QString &code, const QString &base64Image);

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
    void recieve_err_data(QString row);
    void complete_process();

    void on_pb_load_dir_clicked();

private:
    //---Vars
    Ui::DMImportForm *ui;
    DMImportModel* importModel;
    QProgressDialog *progressDialog;
    DoubleProgressDialog *m_doubleProgressDialog;

    QString pdf_importer_path = gSettings.getAppPath() + "/process/extract_datamatrix.exe";
    QString lastUsedDirectory = QDir::homePath();
    // CRUDDMCode db = CRUDDMCode("QSQLITE", gSettings.getAppPath() + "/mydb.sqlite");
    CRUDDMCode db = CRUDDMCode("QSQLITE", "C:/Users/Wepal/Documents/mydb.sqlite");

    // QSqlDatabase* m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));


    //---Funcs
    void startReadDm(const QString &program, const QStringList &arguments);
    void setupImportTable();
    static bool writeImageToDisk(const QString &code, const QString &base64Image);
    static QString getHashForCode(const QString &code);
};

#endif // DMIMPORTFORM_H
