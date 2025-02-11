#include "dmimportform.h"
#include "dminfoform.h"
#include "ui_dmimportform.h"

#include <QFileDialog>
#include <QProcess>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QInputDialog>


#include "widgets/dminfoform.h"
#include "sqlmodels/dmcodemodel.h"
#include "dialogs/invaliddmcodesinfodialog.h"

#include <core/messager.h>

#define gtinIsNotInDb "false"

DMImportForm::DMImportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMImportForm)
    , importModel(new DMImportModel(this))
    , httpManager(new HttpManager(this))
{
    ui->setupUi(this);
    //ui->pb_load_in_db->setEnabled(false);
    ui->pb_load_in_db->setFocusPolicy(Qt::NoFocus);

    connect(importModel, &DMImportModel::dataHasBeenAdded,
            [this](){ui->pb_load_in_db->setEnabled(true);});
    connect(importModel, &DMImportModel::dataHasBeenCleared,
            [this](){ui->pb_load_in_db->setEnabled(true);});

    connect(httpManager, &HttpManager::requestError, [&](const QString& err){
        QMessageBox::critical(this, "Ошибка с БД", err);
    });

    setAcceptDrops(true);
    setupImportTable();

    if (db.createTable<DMCodeModel>()) {
        qDebug() << "User table created successfully";
    } else {
        qDebug() << "Failed to create DMCodeModel table";
    }

    // m_db->setDatabaseName(gSettings.getAppPath()+"/mydb.sqlite");
    // m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    // m_db->setDatabaseName(gSettings.getAppPath() + "/mydb.sqlite");

    // if (!m_db->open()) {
    //     qDebug() << "Error opening database:" << m_db->lastError().text();
    // }
}

DMImportForm::~DMImportForm()
{
    delete ui;
    delete httpManager;
}


void DMImportForm::saveImage(const QString &code, const QString &base64Image)
{
    QFuture<bool> future = QtConcurrent::run(&DMImportForm::writeImageToDisk, code, base64Image);

    // Опционально: если нужно отслеживать завершение операции
    // QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    // watcher->setFuture(future);
    // connect(watcher, &QFutureWatcher<void>::finished, this, &ImageManager::onImageSaved);
}
//-----------------------PRIVATE SLOTS-----------------------
void DMImportForm::on_pb_load_file_clicked()
{
    QStringList gsPath;
    //qDebug() << gSettings.getGsWin64Path();
    gsPath << "--gs-path" << gSettings.getGsWin64Path();
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Открыть файл"),
        lastUsedDirectory,
        tr("PDF и EPS файлы (*.pdf *.eps);;PDF файлы (*.pdf);;EPS Файлы (*.eps);;Все файлы (*)")
        );
    qDebug() << filePaths;
    if((filePaths.size()) > 100){
        showBigAmountWarning();
        return;
    }
    if (!filePaths.isEmpty()) {
        lastUsedDirectory = QFileInfo(filePaths.first()).path();
        gsPath.append(filePaths);
        //qDebug() << gSettings.getDataMatrixExtractPath();
        startReadDm(gSettings.getDataMatrixExtractPath(), gsPath);
    }
}

void DMImportForm::on_pb_load_dir_clicked()
{
    QStringList gsPath;
    gsPath << "--gs-path" << gSettings.getGsWin64Path();
    // qDebug() << gSettings.getDataMatrixExtractPath();
    // qDebug() << gSettings.getGsWin64Path();

    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        tr("Выберите директорию"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (!selectedDir.isEmpty()) {
        // lastUsedDirectory = QFileInfo(selectedDir.first()).path();
        gsPath.append(selectedDir);
        startReadDm(gSettings.getDataMatrixExtractPath(), gsPath);
    }
}

void DMImportForm::init_process()
{
    qDebug() << "init_process";
    // progressDialog = new QProgressDialog("Импорт кодов...", "Отмена", 0, 100, this);

    // progressDialog->setWindowModality(Qt::WindowModal);
    // progressDialog->setMinimumDuration(0);
    // progressDialog->setValue(0);
    // progressDialog->exec();
    m_doubleProgressDialog = new DoubleProgressDialog();
    m_doubleProgressDialog->exec();

    ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::ResizeToContents);
}

void DMImportForm::recieve_dm_data(QString row)
{
    // qDebug() << row;
    row.replace("Payload: ", "");
    QStringList cols = row.split("|");

    quint8 docs_progress = cols.value(0).toUInt(); //Прогресс всех документов
    quint8 doc_progress = cols.value(1).toUInt(); //Прогресс в одном документе
    QString dm_code = cols.value(2);
    QString file_path = cols.value(3);
    // QString img_base64 = cols.value(4);

    // qDebug() << "Doc progress: " << doc_progress << "; Docs progress: " << docs_progress;

    // auto x = 1;
    // if (progressDialog->value()<100) {
    //     progressDialog->setValue(docs_progress);
    // }

    // if (m_doubleProgressDialog->getFileProgress()<100) {
    //     m_doubleProgressDialog->setFileProgress(doc_progress);
    // }

    // DMCodeModel new_code(code, QString("%1.png").arg(getHashForCode(code)));

    // QSqlQuery query(*m_db);

    // auto dt = QDateTime::currentDateTime();
    // QString request = QString("INSERT INTO %1 (code, img_path, import_date) VALUES ('sss', %2, '2024-12-23 11:48:42')")
    //                       .arg("dmcodes", code, getHashForCode(code), dt.toString());
    // query.prepare(request);

    // qDebug() << "Exec query: " << query.exec();
    // if (db.insert(new_code)) {
    //     qDebug() << "New code inserted successfully";
    // } else {
    //     qDebug() << "New code not inserted";
    // }
    // saveImage(dm_code, img_base64);

    DMInfoForm::DataMatrixAttrs dmAttrs;
    bool dmValidated = DMInfoForm::validateDataMatrix(dm_code, dmAttrs);

    if(dmValidated) {
        QString gtin = dmAttrs.gtin;
        importModel->addRow(gtin, dm_code, getFileNameFromPath(file_path), "");
    } else {
        invalideDmCodesPaths.insert(file_path);
    }

    m_doubleProgressDialog->setFileProgress(doc_progress);
    // if (m_doubleProgressDialog->getFilesProgress()<100) {
    //     m_doubleProgressDialog->setFilesProgress(docs_progress);
    // }
    m_doubleProgressDialog->setFilesProgress(docs_progress);
    // if (progressDialog->wasCanceled()) {
    //     // Здесь логика для прерывания процесса
    // }
}

void DMImportForm::recieve_err_data(QString row)
{
    qDebug() << row;
}

void DMImportForm::complete_process()
{
    qDebug() << "Complete process";

    // progressDialog->close();
    // connect(progressDialog, &QProgressDialog::destroyed, this, [this](){
    //     ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::Interactive);;
    //     qDebug() << "Progress dialog has been destroyed";
    // });
    // progressDialog->deleteLater();
    m_doubleProgressDialog->setFileProgress(100);
    m_doubleProgressDialog->setFilesProgress(100);
    m_doubleProgressDialog->close();
    connect(m_doubleProgressDialog, &QProgressDialog::destroyed, this, [this](){
        ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::Interactive);;
        qDebug() << "Progress dialog has been destroyed";
    });
    m_doubleProgressDialog->deleteLater();

    if(!invalideDmCodesPaths.isEmpty()){
        Messager::instance().sendMessage("lkslkgjdklsfj");
        // InvalidDmCodesInfoDialog d(invalideDmCodesPaths);
        // d.exec();
    }
    invalideDmCodesPaths.clear();

    // connect(progressDialog, &QProgressDialog::c)
    // if(progressDialog != nullptr) {
    //     delete progressDialog;
    //     progressDialog = nullptr;
    // }
}

void DMImportForm::files_were_dropped(QStringList filePaths, QStringList dirs)
{
    if((filePaths.size() == 0) && (dirs.size() == 0))
            return;
    if((filePaths.size()+dirs.size()) > 100){
        showBigAmountWarning();
        return;
    }
    QStringList gsPath;
    gsPath << "--gs-path" << gSettings.getGsWin64Path();
    if(filePaths.size() != 0)
        lastUsedDirectory = QFileInfo(filePaths.first()).path();
    else
        lastUsedDirectory = QFileInfo(dirs.first()).path();

    gsPath.append(filePaths);
    gsPath.append(dirs);
    //qDebug() << gsPath;
    startReadDm(gSettings.getDataMatrixExtractPath(), gsPath);
}


//-----------------------PRIVATE SLOTS-----------------------
void DMImportForm::startReadDm(const QString &program, const QStringList &arguments)
{
    importModel->clear();

    QFuture<void> future = QtConcurrent::run([=]() {
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);
        // TODO сделать загрузку нескольких файлов
        process.start(program, arguments);

        if (process.waitForStarted())
        {
            QMetaObject::invokeMethod(this, "init_process",
                                      Qt::QueuedConnection);

            QByteArray buffer;
            while (process.state() == QProcess::Running) {
                if (process.waitForReadyRead(100)) { // Wait for 100ms
                    buffer += process.readAll();
                    int lineEnd;
                    while ((lineEnd = buffer.indexOf('\n')) != -1) {
                        QByteArray line = buffer.left(lineEnd);
                        buffer = buffer.mid(lineEnd + 1);

                        if (!line.isEmpty()) {
                            QString lineStr = QString::fromUtf8(line).trimmed();
                            if(lineStr.startsWith("Payload: ")) {
                                QMetaObject::invokeMethod(this, "recieve_dm_data",
                                                          Qt::QueuedConnection, Q_ARG(QString, lineStr));
                            } else {
                                QMetaObject::invokeMethod(this, "recieve_err_data",
                                                          Qt::QueuedConnection, Q_ARG(QString, lineStr));
                            }
                        }
                    }
                }
            }
            QMetaObject::invokeMethod(this, "complete_process", Qt::QueuedConnection);
        }
        else
        {
            QMetaObject::invokeMethod(this, "processError", Qt::QueuedConnection,
                                      Q_ARG(QString, "Failed to start process: " + process.errorString()));
        }
    });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
    connect(watcher, &QFutureWatcher<void>::destroyed, this, [](){qDebug() << "Watcher destroyed";});
}

// void DMImportForm::startReadDm(const QString &program, const QStringList &arguments)
// {
//     importModel->clear();

//     QFuture<void> future = QtConcurrent::run([=]() {
//         QProcess process;
//         process.setProcessChannelMode(QProcess::SeparateChannels);
//         process.start(program, arguments);

//         if (process.waitForStarted())
//         {
//             QMetaObject::invokeMethod(this, "init_process",
//                                       Qt::QueuedConnection);

//             QByteArray stdoutBuffer;
//             QByteArray stderrBuffer;

//             while (process.state() == QProcess::Running) {
//                 process.waitForReadyRead(100); // Wait for 100ms

//                 // Handle stdout
//                 if (process.bytesAvailable()) {
//                     stdoutBuffer += process.readAllStandardOutput();
//                     int lineEnd;
//                     while ((lineEnd = stdoutBuffer.indexOf('\n')) != -1) {
//                         QByteArray line = stdoutBuffer.left(lineEnd);
//                         stdoutBuffer = stdoutBuffer.mid(lineEnd + 1);

//                         if (!line.isEmpty()) {
//                             QString lineStr = QString::fromUtf8(line).trimmed();
//                             QMetaObject::invokeMethod(this, "recieve_dm_data",
//                                                       Qt::QueuedConnection, Q_ARG(QString, lineStr));
//                         }
//                     }
//                 }

//                 // Handle stderr
//                 if (process.bytesAvailable()) {
//                     stderrBuffer += process.readAllStandardError();
//                     int lineEnd;
//                     while ((lineEnd = stderrBuffer.indexOf('\n')) != -1) {
//                         QByteArray line = stderrBuffer.left(lineEnd);
//                         stderrBuffer = stderrBuffer.mid(lineEnd + 1);

//                         if (!line.isEmpty()) {
//                             QString lineStr = QString::fromUtf8(line).trimmed();
//                             QMetaObject::invokeMethod(this, "recieve_err_data",
//                                                       Qt::QueuedConnection, Q_ARG(QString, lineStr));
//                         }
//                     }
//                 }
//             }
//             QMetaObject::invokeMethod(this, "complete_process", Qt::QueuedConnection);
//         }
//         else
//         {
//             QMetaObject::invokeMethod(this, "processError", Qt::QueuedConnection,
//                                       Q_ARG(QString, "Failed to start process: " + process.errorString()));
//         }
//     });

//     QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
//     watcher->setFuture(future);
//     connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
//     connect(watcher, &QFutureWatcher<void>::destroyed, this, [](){qDebug() << "Watcher destroyed";});
// }

void DMImportForm::setupImportTable()
{
    // ui->tw_dm_codes->setToolTip("");
    // ui->tw_dm_codes->viewport()->setToolTip("");

    // Использование:
    // ui->tw_dm_codes->setItemDelegate(new NoEmptyAreaTooltipDelegate(ui->tw_dm_codes));

    ui->tw_dm_codes->setModel(importModel);
    ui->tw_dm_codes->hideColumn(DMImportModel::ImgColumn);
    ui->tw_dm_codes->horizontalHeader()->setStretchLastSection(true); // растянуть последнюю секцию (это свойство можно задать и просто в дизайнере)

    ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::Interactive);
    ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::ResizeToContents);
    ui->tw_dm_codes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_dm_codes->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw_dm_codes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tw_dm_codes->horizontalHeader()->setFocusPolicy(Qt::NoFocus);

    connect(ui->tw_dm_codes, &DragDropTableView::filesWereDropped, this, &DMImportForm::files_were_dropped);
    connect(ui->tw_dm_codes, &QTableView::doubleClicked, [this](const QModelIndex &index) {
        // Получаем модель
        QAbstractItemModel *model = ui->tw_dm_codes->model();

        // Получаем номер строки
        int row = index.row();

        // Создаем список для хранения значений всех столбцов
        QStringList rowData;


        QModelIndex codeIdx = model->index(row, DMImportModel::CodeColumn);
        QModelIndex filenameIdx = model->index(row, DMImportModel::FilenameColumn);
        // QModelIndex imgIdx = model->index(row, DMImportModel::ImgColumn);
        // qDebug() << "Page: " << model->data(pageIdx).toInt() << "; Code: " << model->data(codeIdx).toString();

        // auto x = db.findById<DMCodeModel>(1);

        // auto entity = db.findByCode<DMCodeModel>(model->data(codeIdx).toString());
        // std::unique_ptr<DMCodeModel> entity = db.findByCode<DMCodeModel>(model->data(codeIdx).toString());
        // QString raw_ptr = entity->m_code;
        // DMCodeModel value = *raw_ptr;

        // if (entity) {
        //     // Объект найден
        //     qDebug() << "Found entity with code:" << entity->m_code;
        // } else {
        //     // Объект не найден
        //     qDebug() << "Entity not found";
        // }


        QDialog dialog;


        QPixmap pixmap(gSettings.getAppPath() + "/DataMatrixImages/" + getHashForCode(model->data(codeIdx).toString()));
        QWidget *widget;
        if (pixmap.isNull()) {
            // widget = new DMInfoForm(model->data(codeIdx).toString(), QString(), &dialog);
            widget = new DMInfoForm(model->data(codeIdx).toString(), &dialog);
        } else {
            // Использование загруженного изображения
            // widget = new DMInfoForm(model->data(codeIdx).toString(), pixmap, &dialog);
            widget = new DMInfoForm(model->data(codeIdx).toString(), &dialog);
        }

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->addWidget(widget);
        dialog.setLayout(layout);

        // Настройка вашего виджета

        int result = dialog.exec();
    });
}

void DMImportForm::showBigAmountWarning()
{
    QMessageBox::warning(this,
                         tr("Внимание!"),
                         tr("Нельзя загрузить более 100 объектов(папок и файлов) за раз! \n"
                            " Используйте папки для большого количества файлов."),
                         QMessageBox::Ok);
}

QString DMImportForm::getFileNameFromPath(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.fileName();
}


bool DMImportForm::writeImageToDisk(const QString &code, const QString &base64Image)
{
    // Создаем директорию для сохранения, если она не существует
    QString saveDir = gSettings.getAppPath() + "/DataMatrixImages";
    QDir().mkpath(saveDir);

    QString codeHash = getHashForCode(code);
    QString fileName = saveDir + "/" + codeHash + ".png";
    QByteArray imageData = QByteArray::fromBase64(base64Image.toUtf8());

    if (QFile::exists(fileName)) {
        qDebug() << "Image already exists:" << fileName;
        return true;  // Возвращаем true, так как файл уже существует
    }

    QImage image;
    if (!image.loadFromData(imageData)) {
        qDebug() << "Failed to load image from data";
        return false;
    }

    if (!image.save(fileName, "PNG")) {
        qDebug() << "Failed to save image to" << fileName;
        return false;
    }

    qDebug() << "Image saved successfully to" << fileName;
    return true;
}

QString DMImportForm::getHashForCode(const QString &code)
{
    return QCryptographicHash::hash(code.toUtf8(), QCryptographicHash::Sha256).toHex();
}

void DMImportForm::insertGtinInDb(const QString& gtin) {
    while (true) {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Ввод имени товара"),
                                         tr("Введите имя для GTIN: %1").arg(gtin),
                                         QLineEdit::Normal, "", &ok);

    if (!ok) {
        QMessageBox::warning(this, tr("Отмена загрузки"),
                             tr("Загрузка отменена пользователем."));
        return;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"),
                             tr("Имя товара не может быть пустым. Пожалуйста, введите имя."));
        continue;
    }

    // Если имя введено корректно, отправляем запрос
    QJsonObject data{{"code", gtin}, {"name", name}};
    QUrl url = HttpManager::createApiUrl("add-gtin");
    httpManager->makeRequest(url,  QJsonDocument(data), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData){
        qDebug() << "GTIN:" << gtin << ", Name: " << name << " - Response: " << responseData;
    });

    break; // Выходим из цикла после успешного ввода и отправки запроса
    }
}

void DMImportForm::insertAllGtinsAndDmCodes() {
    QSet<QString> gtins = importModel->getAllGtins();

    for (const QString& gtin : gtins) {
        QUrl url = HttpManager::createApiUrl("is-gtin/");
        url.setPath(url.path() + QUrl::toPercentEncoding(gtin));
        httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Get, [&](const QByteArray& responseData){
            if (responseData == gtinIsNotInDb) {
                insertGtinInDb(gtin);
            }
        });
    }
    insertAllDmCodes();
}

void DMImportForm::insertAllDmCodes() {
    QJsonArray arr;
    for (const QString& dmCode : importModel->getAllDmCodes()) {
        arr.append(QJsonObject{{"dm_code", dmCode}});
    }

    QUrl url = HttpManager::createApiUrl("add-dmcodes");
    httpManager->makeRequest(url, QJsonDocument(arr), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData){
        QMessageBox::information(this, "Успешная загрузка DM-кодов", "DM-коды были успешно загружены в БД!");
    });
}

void DMImportForm::on_pb_load_in_db_clicked()
{
    insertAllGtinsAndDmCodes();
}

