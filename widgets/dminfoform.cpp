#include "dminfoform.h"
#include "ui_dminfoform.h"

#include <QRegularExpression>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include <dmtx.h>

// DMInfoForm::DMInfoForm(const QString &dmCode, const QString &base64Image, QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::DMInfoForm)
// {
//     ui->setupUi(this);

//     ui->lbl_img->setFixedSize(120, 120); // Установите нужный размер
//     ui->lbl_img->setScaledContents(true);


//     if(base64Image != QString("")) {
//         QByteArray imageData = QByteArray::fromBase64(base64Image.toUtf8());
//         QImage image;
//         image.loadFromData(imageData);
//         ui->lbl_img->setPixmap(QPixmap::fromImage(image));
//     } else {

//         // QLabel *image_label = new QLabel();
//         // image_label->setPixmap(bls_pixmap);
//         // image_label->setScaledContents(true);
//         // image_label->setFixedSize(120, 60); // Установите нужный размер


//         // ":/images/img/bls_logo.png"
//         ui->lbl_img->setPixmap(QPixmap(":/images/img/img_placeholder.png"));
//     }
//     // ui->lbl_img->setFixedSize(600, 600); // Установите нужный размер

//     ui->le_dm_code->setText(dmCode);

//     DataMatrixAttrs dmAttrs = parseDataMatrix(dmCode);
//     DataMatrixAttrs attrs;
//     bool x = validateDataMatrix(dmCode, attrs);
//     setDMFields(dmAttrs);
// }

// DMInfoForm::DMInfoForm(const QString &dmCode, const QPixmap &image, QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::DMInfoForm)
// {
//     ui->setupUi(this);

//     ui->lbl_img->setFixedSize(120, 120); // Установите нужный размер
//     ui->lbl_img->setScaledContents(true);

//     ui->lbl_img->setPixmap(image);
//     ui->le_dm_code->setText(dmCode);

//     DataMatrixAttrs dmAttrs = parseDataMatrix(dmCode);
//     setDMFields(dmAttrs);
// }

DMInfoForm::DMInfoForm(const QString &dmCode, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DMInfoForm)
{
    ui->setupUi(this);

    ui->lbl_img->setFixedSize(120, 120); // Установите нужный размер
    ui->lbl_img->setScaledContents(true);
    ui->le_dm_code->setText(dmCode);

    DataMatrixAttrs dmAttrs;
    bool dmValidated = validateDataMatrix(dmCode, dmAttrs);

    if(dmValidated) {
        setDMFields(dmAttrs);
        QString code = prepareDataMatrix(dmCode);
        QPixmap img = generateDataMatrix(code, 10);
        ui->lbl_img->setPixmap(img);
    }
    // setDMFields(dmAttrs);
    QString code = prepareDataMatrix(dmCode);
    QPixmap img = generateDataMatrix(code, 10);
    ui->lbl_img->setPixmap(img);

}

DMInfoForm::~DMInfoForm()
{
    delete ui;
}

//-----------------------------PUBLIC-----------------------------
DMInfoForm::DataMatrixAttrs DMInfoForm::parseDataMatrix(const QString &dataMatrix)
{
    DataMatrixAttrs result;

    // Нормализуем код, заменяя QChar(29) на "<GS>"
    QString cleanCode = normalizeGS(dataMatrix);

    // Разделяем код на группы
    QStringList groups = cleanCode.split("<GS>");
    qDebug() << groups;


    for (const QString& group : groups) {
        if (group.startsWith("01")) {
            result.gtin = group.mid(2, 14);
            if (group.length() > 16) {  // Если после GTIN есть еще данные
                QString remainder = group.mid(16);
                if (remainder.startsWith("21")) {

                    if (remainder.at(2).isDigit()) {
                        QString countryCode = remainder.mid(2, 1);

                        result.countryCode = countryCode.toInt();
                        result.country = countryToString(static_cast<Country>(result.countryCode));

                        result.serialNumber = remainder.mid(3, -1);
                    } else {
                        QString countryCode = 0;

                        result.countryCode = countryCode.toInt();
                        result.country = countryToString(static_cast<Country>(result.countryCode));
                        result.serialNumber = remainder.mid(2, -1);
                    }
                }
            }
        } else if (group.startsWith("91")) {
            result.verificationKey = group.mid(2, 4);
            result.isLongFormat = true;
        } else if (group.startsWith("92")) {
            result.verificationKeyValue = group.mid(2);
            result.isLongFormat = true;
        } else if (group.startsWith("93")) {
            result.verificationKey = group.mid(2, 4);
            result.isLongFormat = false;
        }
    }
    return result;
}

bool DMInfoForm::validateDataMatrix(const QString &dataMatrix, DataMatrixAttrs& attrs)
{
    // Нормализуем код перед проверкой
    QString normalizedCode = normalizeGS(dataMatrix);

    // Регулярные выражения для проверки формата
    // QRegularExpression longFormatRegex("^(?:\\x{00E8})?01\\d{14}21[1-5].{5}<GS>91.{4}<GS>92.{44}$");
    QRegularExpression longFormatRegex(R"(^01\d{14}21[0-5].{5,12}<GS>91.{4}<GS>92.{44}$)");
    // QRegularExpression shortFormatRegex("^(?:\\x{00E8})?01\\d{14}21[1-5].{5}<GS>93.{4}$");
    QRegularExpression shortFormatRegex(R"(^01\d{14}21[0-5].{5,12}<GS>93.{4}$)");

    QRegularExpression longFormatRegexNoCountry(R"(^01\d{14}21[A-Za-z].{5,13}<GS>91.{4}<GS>92.{44}$)");
    // QRegularExpression shortFormatRegex("^(?:\\x{00E8})?01\\d{14}21[1-5].{5}<GS>93.{4}$");
    QRegularExpression shortFormatRegexNoCountry(R"(^01\d{14}21[A-Za-z].{5,13}<GS>93.{4}$)");

    if (longFormatRegex.match(normalizedCode).hasMatch() || shortFormatRegex.match(normalizedCode).hasMatch()) {
        attrs = parseDataMatrix(normalizedCode);

        // Проверка GTIN
        if (attrs.gtin.length() != 14 || !attrs.gtin.toULongLong()) return false;

        // Проверка серийного номера
        if (attrs.serialNumber.length() < 5 || attrs.serialNumber.length() > 13) return false;

        // Проверка кода страны
        if (attrs.countryCode < 0 || attrs.countryCode > 5) return false;

        // Проверка формата и соответствующих полей
        if (attrs.isLongFormat) {
            if (attrs.verificationKey.length() != 4) return false;
            if (attrs.verificationKeyValue.length() != 44) return false;

        } else {
            if (attrs.verificationKey.length() != 4) return false;
            if (attrs.verificationKeyValue != QString("-")) return false;
        }

        return true;
    } else if (longFormatRegexNoCountry.match(normalizedCode).hasMatch() || shortFormatRegexNoCountry.match(normalizedCode).hasMatch()) {
        attrs = parseDataMatrix(normalizedCode);

        // Проверка GTIN
        if (attrs.gtin.length() != 14 || !attrs.gtin.toULongLong()) return false;

        // Проверка серийного номера
        if (attrs.serialNumber.length() < 5 || attrs.serialNumber.length() > 13) return false;

        // Проверка кода страны
        if (attrs.countryCode < 0 || attrs.countryCode > 5) return false;

        // Проверка формата и соответствующих полей
        if (attrs.isLongFormat) {
            if (attrs.verificationKey.length() != 4) return false;
            if (attrs.verificationKeyValue.length() != 44) return false;

        } else {
            if (attrs.verificationKey.length() != 4) return false;
            if (attrs.verificationKeyValue != QString("-")) return false;
        }
        return true;
    }

    return false;
}


QString DMInfoForm::prepareDataMatrix(const QString &code)
{
    QString result = QChar(29);
    result.append(code).replace("<GS>", QChar(29));
    return result;
}

QPixmap DMInfoForm::generateDataMatrix(const QString& data, int size)
{
    DmtxEncode *enc = dmtxEncodeCreate();
    dmtxEncodeSetProp(enc, DmtxPropPixelPacking, DmtxPack24bppRGB);
    dmtxEncodeSetProp(enc, DmtxPropWidth, size);
    dmtxEncodeSetProp(enc, DmtxPropHeight, size);

    QByteArray dataBytes = data.toUtf8();
    dmtxEncodeDataMatrix(enc, dataBytes.length(), (unsigned char*)dataBytes.data());

    int width = dmtxImageGetProp(enc->image, DmtxPropWidth);
    int height = dmtxImageGetProp(enc->image, DmtxPropHeight);

    QImage image(width, height, QImage::Format_RGB32);
    int bytesPerPixel = dmtxImageGetProp(enc->image, DmtxPropBytesPerPixel);
    int rowPadBytes = dmtxImageGetProp(enc->image, DmtxPropRowPadBytes);
    int rowSizeBytes = dmtxImageGetProp(enc->image, DmtxPropRowSizeBytes);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int offset = y * rowSizeBytes + x * bytesPerPixel;
            unsigned char *pxl = enc->image->pxl + offset;
            image.setPixel(x, y, qRgb(pxl[0], pxl[1], pxl[2]));
        }
    }

    dmtxEncodeDestroy(&enc);

    return QPixmap::fromImage(image);
}

//-----------------------------PRIVATE-----------------------------
void DMInfoForm::setDMFields(const DataMatrixAttrs &attrs)
{
    ui->le_gtin->setText(attrs.gtin);
    ui->le_country_code->setText(QString::number(attrs.countryCode));

    ui->le_country->setText(attrs.country);
    ui->le_serial->setText(attrs.serialNumber);
    ui->le_verify_code->setText(attrs.verificationKey);
    ui->le_verify_code_value->setText(attrs.verificationKeyValue);
}

QString DMInfoForm::normalizeGS(QString input)
{


    // Удаляем символ FNC1 в начале, если он есть
    if (input.startsWith(QChar(232)) || input.startsWith(QChar(29)))
        input.remove(0, 1);

    return input.replace(QChar(29), "<GS>");
}
