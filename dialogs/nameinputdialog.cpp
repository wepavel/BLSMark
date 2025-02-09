#include "nameinputdialog.h"
#include "ui_nameinputdialog.h"

#include <QMessageBox>

NameInputDialog::NameInputDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NameInputDialog)
{
    ui->setupUi(this);
}

QString NameInputDialog::getName() const
{
    return name;
}

NameInputDialog::~NameInputDialog()
{
    delete ui;
}

void NameInputDialog::on_pb_add_clicked()
{
    if(ui->le_name->text().isEmpty()){
        QMessageBox::warning(this, tr("Внимание"),tr("Заполните все поля!"));
        return;
    }
    this->name = ui->le_name->text();
    done(QDialog::Accepted);
}

void NameInputDialog::on_pb_cancel_clicked()
{
    done(QDialog::Rejected);
}

