#include "dmimportform.h"
#include "ui_dmimportform.h"
#include <QDir>
#include <QFileDialog>
#include <QProcess>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "widgets/dminfoform.h"

DMImportForm::DMImportForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMImportForm)
    , importModel(new DMImportModel(this))
{
    ui->setupUi(this);

    setAcceptDrops(true);
    setupImportTable();
}

DMImportForm::~DMImportForm()
{
    delete ui;
}


//-----------------------PRIVATE SLOTS-----------------------
void DMImportForm::on_pb_load_file_clicked()
{
    QStringList file_paths = QFileDialog::getOpenFileNames(
        this,
        tr("Открыть файл"),
        QDir::homePath(),
        tr("PDF файлы (*.pdf);;EPS Файлы (*.eps)")
        );
    if (!file_paths.isEmpty()) {
        startReadDm(pdf_importer_path, file_paths);
    }
}

void DMImportForm::init_process()
{
    qDebug() << "init_process";
    progressDialog = new QProgressDialog("Импорт кодов...", "Отмена", 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);
    progressDialog->setValue(0);
    progressDialog->exec();
}

void DMImportForm::recieve_dm_data(QString row)
{
    // qDebug() << row;
    QStringList cols = row.split("|");

    if (progressDialog->value()<100) {
        progressDialog->setValue(cols.value(2).toInt());
    }

    importModel->addRow(cols.value(3), cols.value(1).toInt(), cols.value(4));

    // if (progressDialog->wasCanceled()) {
    //     // Здесь логика для прерывания процесса
    // }
}

void DMImportForm::complete_process()
{
    qDebug() << "Complete process";

    progressDialog->close();
    if(progressDialog != nullptr) {
        delete progressDialog;
        progressDialog = nullptr;
    }
}
//-----------------------PRIVATE SLOTS-----------------------
void DMImportForm::startReadDm(const QString &program, const QStringList &arguments)
{
    importModel->clear();

    QFuture<void> future = QtConcurrent::run([=]() {
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);
        // TODO сделать загрузку нескольких файлов
        process.start(program, QStringList() << arguments.value(0));

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
                            QMetaObject::invokeMethod(this, "recieve_dm_data",
                                                      Qt::QueuedConnection, Q_ARG(QString, lineStr));
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
    ui->tw_dm_codes->setModel(importModel);
    ui->tw_dm_codes->hideColumn(DMImportModel::ImgColumn);
    ui->tw_dm_codes->horizontalHeader()->setStretchLastSection(true); // растянуть последнюю секцию (это свойство можно задать и просто в дизайнере)
    ui->tw_dm_codes->horizontalHeader()->setSectionResizeMode(DMImportModel::NumPageColumn, QHeaderView::ResizeToContents);
    ui->tw_dm_codes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_dm_codes->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw_dm_codes->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->tw_dm_codes, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        // Получаем модель
        QAbstractItemModel *model = ui->tw_dm_codes->model();

        // Получаем номер строки
        int row = index.row();

        // Создаем список для хранения значений всех столбцов
        QStringList rowData;

        QModelIndex pageIdx = model->index(row, DMImportModel::NumPageColumn);
        QModelIndex codeIdx = model->index(row, DMImportModel::CodeColumn);
        QModelIndex imgIdx = model->index(row, DMImportModel::ImgColumn);
        qDebug() << "Page: " << model->data(pageIdx).toInt() << "; Code: " << model->data(codeIdx).toString();

        QDialog dialog;
        QWidget *widget = new DMInfoForm(model->data(codeIdx).toString(), model->data(imgIdx).toString(), &dialog);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->addWidget(widget);
        dialog.setLayout(layout);

        // Настройка вашего виджета

        int result = dialog.exec();
    });
}

