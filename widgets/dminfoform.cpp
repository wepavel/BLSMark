#include "dminfoform.h"
#include "ui_dminfoform.h"

DMInfoForm::DMInfoForm(const QString &dm_code, const QString &img_base64, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMInfoForm)
{
    ui->setupUi(this);

    ui->lbl_img->setFixedSize(120, 120); // Установите нужный размер
    ui->lbl_img->setScaledContents(true);


    if(img_base64 != QString("")) {
        QByteArray imageData = QByteArray::fromBase64(img_base64.toUtf8());
        QImage image;
        image.loadFromData(imageData);
        ui->lbl_img->setPixmap(QPixmap::fromImage(image));
    } else {

        // QLabel *image_label = new QLabel();
        // image_label->setPixmap(bls_pixmap);
        // image_label->setScaledContents(true);
        // image_label->setFixedSize(120, 60); // Установите нужный размер


        // ":/images/img/bls_logo.png"
         ui->lbl_img->setPixmap(QPixmap(":/images/img/img_placeholder.png"));
    }
    // ui->lbl_img->setFixedSize(600, 600); // Установите нужный размер

    ui->le_dm_code->setText(dm_code);
}

DMInfoForm::~DMInfoForm()
{
    delete ui;
}
