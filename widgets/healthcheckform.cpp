#include "healthcheckform.h"
#include "ui_healthcheckform.h"

HealthCheckForm::HealthCheckForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HealthCheckForm)
{
    ui->setupUi(this);
    QSpacerItem *left_spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout()->addItem(left_spacer);
    layout()->setContentsMargins(0,0,0,0);

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
        if(unitName == "Сервер" && available == false)
        {
            for(const QString &name: unitsOrder){
                unitsMap[name]->setAvailable(false);
                unitsMap[name]->setWorking(false);
            }
        }
        unitsMap[unitName]->setAvailable(available);
    } else {
        qDebug() << "Единица не найдена: " << unitName;
    }
}

void HealthCheckForm::setWorks(const QString& unitName, const bool works)
{
    if (unitsMap.contains(unitName))
    {
        if(unitName == "Сервер" && works == false)
        {
            for(const QString &name: unitsOrder){
                if(name == "Сервер")
                    continue;
                unitsMap[name]->setAvailable(false);
                unitsMap[name]->setWorking(false);
            }
        }
        unitsMap[unitName]->setWorking(works);
    } else {
        qDebug() << "Единица не найдена:" << unitName;
    }
}

QSet<QString> HealthCheckForm::getNotWorkingUnits() const
{
    QSet<QString> result = {};
    foreach (const QString &key, unitsMap.keys()) {
        HealthUnitForm* unit = unitsMap.value(key);
        if(!unit->getItWorks()){
            result.insert(key);
        }
    }
    return result;
}

QSet<QString> HealthCheckForm::getNotAvailUnits() const
{
    QSet<QString> result = {};
    foreach (const QString &key, unitsMap.keys()) {
        HealthUnitForm* unit = unitsMap.value(key);
        if(!unit->getItIsAvailable()){
            result.insert(key);
        }
    }
    return result;
}
