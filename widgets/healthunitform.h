#ifndef HEALTHUNITFORM_H
#define HEALTHUNITFORM_H

#include <QWidget>

namespace Ui {
class HealthUnitForm;
}

class HealthUnitForm : public QWidget
{
    Q_OBJECT

public:
    explicit HealthUnitForm(const QString& unitName, QWidget *parent = nullptr);
    ~HealthUnitForm();
    void setWorking(const bool works);
    void setAvailable(const bool available);

private:
    Ui::HealthUnitForm *ui;
    QString name;
    bool itWorks = false;
    bool itIsAvailable = false;
};

#endif // HEALTHUNITFORM_H
