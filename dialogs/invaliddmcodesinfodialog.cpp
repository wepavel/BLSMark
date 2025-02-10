#include "invaliddmcodesinfodialog.h"
#include "ui_invaliddmcodesinfodialog.h"

InvalidDmCodesInfoDialog::InvalidDmCodesInfoDialog(QSet<QString> invCodes,
                                                   QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InvalidDmCodesInfoDialog)
{
    ui->setupUi(this);
    ui->lw_invalidDmCodes->addItems(invCodes.values());
    ui->lbl_warning->setFocusPolicy(Qt::NoFocus);
    auto p = QPixmap(":/images/img/warning.png").scaled(50, 50, Qt::KeepAspectRatioByExpanding);
    ui->lbl_warning->setPixmap(p);
}

InvalidDmCodesInfoDialog::~InvalidDmCodesInfoDialog()
{
    delete ui;
}
