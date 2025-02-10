#ifndef INVALIDDMCODESINFODIALOG_H
#define INVALIDDMCODESINFODIALOG_H

#include <QDialog>

namespace Ui {
class InvalidDmCodesInfoDialog;
}

class InvalidDmCodesInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvalidDmCodesInfoDialog(QSet<QString> invCodes,
                                      QWidget *parent = nullptr);
    ~InvalidDmCodesInfoDialog();

private:
    Ui::InvalidDmCodesInfoDialog *ui;
};

#endif // INVALIDDMCODESINFODIALOG_H
