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

    QSet<QString> getNotWorkingUnits() const;
    QSet<QString> getNotAvailUnits() const;

private:
    Ui::HealthCheckForm *ui;
    QStringList unitsOrder = {"Сервер", "Сканер", "Принтер", "Аппликатор", "База данных"};
    QMap<QString, HealthUnitForm*> unitsMap;
};

#endif // HEALTHCHECKFORM_H
