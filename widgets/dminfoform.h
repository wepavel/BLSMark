#ifndef DMINFOFORM_H
#define DMINFOFORM_H

#include <QWidget>

namespace Ui {
class DMInfoForm;
}

class DMInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMInfoForm(
        const QString &dm_code,
        const QString &img_base64=QString(),
        QWidget *parent = nullptr
        );
    ~DMInfoForm();

private:
    Ui::DMInfoForm *ui;
};

#endif // DMINFOFORM_H
