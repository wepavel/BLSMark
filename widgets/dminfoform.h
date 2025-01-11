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
        const QString &dmCode,
        const QString &base64Image=QString(),
        QWidget *parent = nullptr
        );
    explicit DMInfoForm(
        const QString &dmCode,
        const QPixmap &image=QPixmap(":/images/img/img_placeholder.png"),
        QWidget *parent = nullptr
        );
    ~DMInfoForm();

private:
    Ui::DMInfoForm *ui;
};

#endif // DMINFOFORM_H
