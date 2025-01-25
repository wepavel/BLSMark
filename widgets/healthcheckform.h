#ifndef HEALTHCHECKFORM_H
#define HEALTHCHECKFORM_H

#include <QWidget>
#include "widgets/healthunitform.h"

namespace Ui {
class HealthCheckForm;
}

class HealthCheckForm : public QWidget
{
    Q_OBJECT

public:
    explicit HealthCheckForm(QWidget *parent = nullptr);
    ~HealthCheckForm();
    void setAvailable(const QString& unitName, const bool available);
    void setWorks(const QString& unitName, const bool works);

private:
    Ui::HealthCheckForm *ui;
    QStringList unitsOrder = {"Сервис", "Сканер", "Принтер", "ПЛК"};
    QMap<QString, HealthUnitForm*> unitsMap;
};

#endif // HEALTHCHECKFORM_H
