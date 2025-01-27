#include "healthunitform.h"
#include "ui_healthunitform.h"

HealthUnitForm::HealthUnitForm(const QString& unitName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HealthUnitForm)
{
    ui->setupUi(this);
    name = unitName;
    ui->lbl_title->setText(name+":");
    setWorking(false);
    setAvailable(false);
}

HealthUnitForm::~HealthUnitForm()
{
    delete ui;
}

void HealthUnitForm::setWorking(const bool works)
{
    if(works){
        ui->lbl_works->setText("Работает");
        ui->lbl_works->setStyleSheet("color: green");
        itWorks = true;
    } else {
        ui->lbl_works->setText("Не работает");
        ui->lbl_works->setStyleSheet("color: red");
        itWorks = false;
    }
}

void HealthUnitForm::setAvailable(const bool available)
{
    if(available){
        ui->lbl_available->setText("Доступен");
        ui->lbl_available->setStyleSheet("color: green");
        itIsAvailable = true;
    } else {
        ui->lbl_available->setText("Не доступен");
        ui->lbl_available->setStyleSheet("color: red");
        itIsAvailable = false;
    }
}
