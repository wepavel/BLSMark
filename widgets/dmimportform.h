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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include "core/httpmanager.h"
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

private slots:
    //-----------------------PRIVATE SLOTS-----------------------
    void on_pb_load_file_clicked();
    void init_process();
    void recieve_dm_data(QString row);
    void recieve_err_data(QString row);
    void complete_process();
    void files_were_dropped(QStringList filePaths, QStringList dirs);
    void on_pb_load_dir_clicked();
    void on_pb_load_in_db_clicked();

private:
    //---Vars
    Ui::DMImportForm *ui;
    DMImportModel* importModel;
    QProgressDialog *progressDialog;
    DoubleProgressDialog *m_doubleProgressDialog;
    HttpManager *httpManager;


    QString lastUsedDirectory = QDir::homePath();
    // CRUDDMCode db = CRUDDMCode("QSQLITE", gSettings.getAppPath() + "/mydb.sqlite");
    CRUDDMCode db = CRUDDMCode("QSQLITE", "C:/Users/Wepal/Documents/mydb.sqlite");

    // QSqlDatabase* m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    QSet<QString> invalideDmCodesPaths = {"Путь до файла1", "Путь до файла2", "Путь до файла3","Путь до файла4"};

    // std::atomic<int> requestCount{0};
    // bool dmCodesInserted = false; // Флаг, чтобы гарантировать однократный вызов insertAllDmCodes
    // QMutex dmCodesMutex; // Защита доступа к dmCodesInserted
    QStringList gtinsToInsert = {};

    //---Funcs
    void startReadDm(const QString &program, const QStringList &arguments);
    void setupImportTable();
    void showBigAmountWarning();
    QString getFileNameFromPath(const QString& filePath);
    static bool writeImageToDisk(const QString &code, const QString &base64Image);
    static QString getHashForCode(const QString &code);
    void insertGtinInDb(const QString& gtin);
    void insertAllGtinsAndDmCodes();
    void insertAllDmCodes();

    // QUrl createApiUrl(const QString &endpoint) const {
    //     return QUrl(QString("http://%1:%2/api/v1/code-processing/%3")
    //                     .arg(gSettings.getBackendServiceIP())
    //                     .arg(gSettings.getBackendServicePort())
    //                     .arg(endpoint));
    // }

    // QNetworkReply* sendJsonRequest(const QUrl &url, const QJsonDocument &jsonDoc, bool isPost = true) {
    //     QNetworkRequest request(url);
    //     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    //     if (isPost) {
    //         return httpManager->post(request, jsonDoc.toJson());
    //     } else {
    //         return httpManager->get(request);
    //     }
    // }

    // void handleNetworkReply(QNetworkReply *reply, const std::function<void(const QByteArray&)> &successCallback) {
    //     if (reply->error() == QNetworkReply::NoError) {
    //         QByteArray responseData = reply->readAll();
    //         successCallback(responseData);
    //     } else {
    //         qDebug() << "Network Error: " << reply->errorString();
    //     }
    //     reply->deleteLater();
    // }
};

#endif // DMIMPORTFORM_H


