#include "doubleprogressdialog.h"
#include "ui_doubleprogressdialog.h"

DoubleProgressDialog::DoubleProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DoubleProgressDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowTitle("Идёт импорт файлов");
    ui->prb_load_file->setRange(0,100);
    ui->prb_load_files->setRange(0,100);

    ui->prb_load_file->setValue(0);
    ui->prb_load_files->setValue(0);

    // QProgressDialog("Импорт кодов...", "Отмена", 0, 100, this);
}

DoubleProgressDialog::~DoubleProgressDialog()
{
    delete ui;
}

void DoubleProgressDialog::setFileProgress(quint8 progress)
{
    ui->prb_load_file->setValue(progress);
}

quint8 DoubleProgressDialog::getFileProgress()
{
    return ui->prb_load_file->value();
}

void DoubleProgressDialog::setFilesProgress(quint8 progress)
{
    ui->prb_load_files->setValue(progress);
}

quint8 DoubleProgressDialog::getFilesProgress()
{
    return ui->prb_load_files->value();
}
