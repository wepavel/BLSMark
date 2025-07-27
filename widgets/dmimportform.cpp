#include "dmimportform.h"
#include "dminfoform.h"
#include "ui_dmimportform.h"
#include <QFileDialog>
#include <QProcess>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QTextCodec>
#include "widgets/dminfoform.h"
// #include "sqlmodels/dmcodemodel.h"
#include <core/messager.h>

#define gtinIsNotInDb "false"

DMImportForm::DMImportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMImportForm)
    , importModel(new DMImportModel(this))
    , httpManager(new HttpManager(this))
{
    ui->setupUi(this);
    ui->tw_dm_codes->setFocusPolicy(Qt::NoFocus);
    //ui->pb_load_in_db->setEnabled(false);
    ui->pb_load_in_db->setFocusPolicy(Qt::NoFocus);

    connect(importModel, &DMImportModel::dataHasBeenAdded,
            [this](){ui->pb_load_in_db->setEnabled(true);});
    connect(importModel, &DMImportModel::dataHasBeenCleared,
            [this](){ui->pb_load_in_db->setEnabled(true);});

    setAcceptDrops(true);
    setupImportTable();
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
    qDebug() << "Init import";

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

    DMInfoForm::DataMatrixAttrs dmAttrs;
    bool dmValidated = DMInfoForm::validateDataMatrix(dm_code, dmAttrs);

    if(dmValidated) {
        QString gtin = dmAttrs.gtin;
        QString normalized_dm_code = dmAttrs.dm_code;
        importModel->addRow(gtin, normalized_dm_code, getFileNameFromPath(file_path), "");
    } else {
        invalideDmCodesPaths.insert(file_path);
    }

    m_doubleProgressDialog->setFileProgress(doc_progress);
    m_doubleProgressDialog->setFilesProgress(docs_progress);
}

void DMImportForm::recieve_err_data(QString row)
{
    // qDebug() << row;
    invalideDmCodesPaths.insert(row);
}

void DMImportForm::complete_process()
{
    qDebug() << "Complete import";

    m_doubleProgressDialog->setFileProgress(100);
    m_doubleProgressDialog->setFilesProgress(100);
    m_doubleProgressDialog->close();
    connect(m_doubleProgressDialog, &QProgressDialog::destroyed, this, [this](){
        ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::CodeColumn, QHeaderView::Interactive);;
        qDebug() << "Progress dialog has been destroyed";
    });
    m_doubleProgressDialog->deleteLater();

    if(!invalideDmCodesPaths.isEmpty()){
        for (const QString& path: invalideDmCodesPaths){
            messagerInst.addMessage("Невалидный код: \"" + path + "\" не будет загружен!",
                                            MsgType::Error, true);
        }
    }
    invalideDmCodesPaths.clear();

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

void DMImportForm::on_pb_load_in_db_clicked()
{
    if (importModel->rowCount()<=0) {
        QMessageBox::warning(this, tr("Внимание!"), tr("Невозможно загрузить в\nбазу данных пустую таблицу!"));
        return;
    }
    insertAllGtinsAndDmCodes();
}

//-----------------------PRIVATE FUNCS-----------------------
void DMImportForm::startReadDm(const QString &program, const QStringList &arguments)
{
    // Проверяем существование программы
    QFileInfo programFile(program);

    if (!programFile.exists()) {
        QMessageBox::critical(this,
                              tr("Ошибка"),
                              tr("Программа %1 не найдена!").arg(program));
        return;
    }

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
                            qDebug() << lineStr;
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
        if(!index.isValid()){
            return;
        }
        int row = index.row();
        QModelIndex codeIdx = importModel->index(row, DMImportModel::CodeColumn);
        QString code = importModel->data(codeIdx).toString();
        if(code.isEmpty())
            return;
        DMInfoForm::showInfoDialog(this, code);
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

    QString codeHash = DMInfoForm::getHashForCode(code);
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

bool DMImportForm::insertGtinInDb(const QString& gtin) {
    bool insertGtinSuccess = true; // Флаг для отслеживания успеха
    while (true) {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Ввод имени товара"),
                                         tr("Введите имя для GTIN: %1").arg(gtin),
                                         QLineEdit::Normal, "", &ok);

    if (!ok) {
        QMessageBox::warning(this, tr("Отмена загрузки"),
                             tr("Загрузка отменена пользователем."));
        return false;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"),
                             tr("Имя товара не может быть пустым. Пожалуйста, введите имя."));
        continue;
    }

    // Если имя введено корректно, отправляем запрос
    QJsonObject data{{"code", gtin}, {"name", name}};
    QUrl url = HttpManager::createApiUrl("code-import/add-gtin");
    httpManager->makeRequest(url,  QJsonDocument(data), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){
        if (statusCode!=200 && statusCode!=-1) {
            QMessageBox::warning(this, tr("Внимание!"), tr("Не удалось выполнить запрос!"));
            insertGtinSuccess = false;
        } else if (statusCode==-1) {
            QMessageBox::warning(this, tr("Внимание!"), tr("Не удалось отправить запрос на сервер!"));
            insertGtinSuccess = false;
        }
        qDebug() << "GTIN:" << gtin << ", Name: " << name << " - Response: " << responseData;
    });

    break; // Выходим из цикла после успешного ввода и отправки запроса
    }
    return insertGtinSuccess;
}

void DMImportForm::insertAllGtinsAndDmCodes() {
    QSet<QString> gtins = importModel->getAllGtins();
    bool insertGtinSuccess = true; // Флаг для отслеживания успеха

    for (const QString& gtin : gtins) {
        QUrl url = HttpManager::createApiUrl("code-import/is-gtin/");
        url.setPath(url.path() + QUrl::toPercentEncoding(gtin));
        httpManager->makeRequest(url, QJsonDocument(), HttpManager::HttpMethod::Get, [&](const QByteArray& responseData, int statusCode){
            if (statusCode!=200 && statusCode!=-1) {
                messagerInst.addMessage("Не удалось выполнить запрос! Код ответа: "+QString::number(statusCode)
                                            +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
                insertGtinSuccess = false;
            } else if (statusCode==-1) {
                messagerInst.addMessage("Не удалось выполнить запрос! Код ответа: "+QString::number(statusCode), Error, true);
                insertGtinSuccess = false; // также
            } else {
                if (QString::fromUtf8(responseData) == "false" && !insertGtinInDb(gtin)) {
                    insertGtinSuccess = false; // Устанавливаем флаг в false
                    return; // Выходим из цикла, чтобы не обрабатывать остальные GTIN
                }
            }
        });
        if (!insertGtinSuccess) return;
    }
    insertAllDmCodes();
}

QVariant ObjectOrArrayFromString(const QString& in)
{
    QVariant result;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if (!doc.isNull())
    {
        if (doc.isObject()) {
            result = doc.object();
        } else if (doc.isArray()) {
            result = doc.array();
        } else {
            qDebug() << "Document is neither an object nor an array";
        }
    } else {
        qDebug() << "Invalid JSON...\n" << in;
    }

    return result;
}

void DMImportForm::insertAllDmCodes() {
    ui->pb_load_in_db->setEnabled(false);
    QJsonArray arr;
    for (const QString& dmCode : importModel->getAllDmCodes()) {
        arr.append(QJsonObject{{"dm_code", dmCode}});
    }

    QUrl url = HttpManager::createApiUrl("code-import/add-dmcodes");
    httpManager->makeRequest(url, QJsonDocument(arr), HttpManager::HttpMethod::Post, [&](const QByteArray& responseData, int statusCode){


        if (statusCode!=200 && statusCode!=-1) {
                messagerInst.addMessage("Не удалось выполнить запрос! Код ответа: "+QString::number(statusCode)
                                            +"\n Тело ответа: "+QString::fromUtf8(responseData), Error, true);
        } else if (statusCode==-1) {
            messagerInst.addMessage("Не удалось выполнить запрос! Код ответа: "+QString::number(statusCode), Error, true);
        } else {
            QJsonArray result = ObjectOrArrayFromString(responseData).toJsonArray();
            if (result.isEmpty()) {
                QMessageBox::information(this, tr("Успешная загрузка DM-кодов"), tr("DM-коды были успешно загружены в БД!"));
                messagerInst.addMessage(tr("Успешная загрузка DM-кодов. DM-коды были успешно загружены в БД!"), Info);
            } else {
                for (const QJsonValue &value : qAsConst(result)) {
                    QJsonObject jsonObject = value.toObject();
                    QString message = QString("StatusCode: %1; Ошибка добавления кода %2! %3")
                                          .arg(statusCode)
                                          .arg(jsonObject.value("dm_code").toString())
                                          .arg(jsonObject.value("problem").toString());
                    messagerInst.addMessage(message, Warning, true);
                }
                QMessageBox::warning(this, tr("Внимание!"), tr("Не все коды были успешно загружены!\nОбратите внимание на окно предупреждений!"));
            }
        }
    });
    ui->pb_load_in_db->setEnabled(true);
}



