#include "healthcheckform.h"
#include "ui_healthcheckform.h"

HealthCheckForm::HealthCheckForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HealthCheckForm)
{
    ui->setupUi(this);
    QSpacerItem *left_spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout()->addItem(left_spacer);

    for(const QString &name: unitsOrder){
        unitsMap[name] = new HealthUnitForm(name,this);
        layout()->addWidget(unitsMap[name]);
    }
    QSpacerItem *right_spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout()->addItem(right_spacer);
}

HealthCheckForm::~HealthCheckForm()
{
    delete ui;
}

void HealthCheckForm::setAvailable(const QString& unitName, const bool available)
{
    if (unitsMap.contains(unitName))
    {
        // if(unitName == "Сервис" && available == false)
        // {
        //     for(const QString &name: unitsOrder){
        //         unitsMap[name]->setAvailable(false);
        //         unitsMap[name]->setWorking(false);
        //     }
        // }
        unitsMap[unitName]->setAvailable(available);
    } else {
        qDebug() << "Единица не найдена: " << unitName;
    }
}

void HealthCheckForm::setWorks(const QString& unitName, const bool works)
{
    if (unitsMap.contains(unitName))
    {
        // if(unitName == "Сервис" && works == false)
        // {
        //     for(const QString &name: unitsOrder){
        //         if(name == "Сервис")
        //             continue;
        //         unitsMap[name]->setAvailable(false);
        //         unitsMap[name]->setWorking(false);
        //     }
        // }
        unitsMap[unitName]->setWorking(works);
    } else {
        qDebug() << "Единица не найдена:" << unitName;
    }
}
