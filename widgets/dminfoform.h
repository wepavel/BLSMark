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
    // explicit DMInfoForm(
    //     const QString &dmCode,
    //     const QString &base64Image=QString(),
    //     QWidget *parent = nullptr
    //     );
    // explicit DMInfoForm(
    //     const QString &dmCode,
    //     const QPixmap &image=QPixmap(":/images/img/img_placeholder.png"),
    //     QWidget *parent = nullptr
    //     );
    explicit DMInfoForm(
        const QString &dmCode,
        QWidget *parent = nullptr
        );
    ~DMInfoForm();

    //---Structs
    struct DataMatrixAttrs
    {
        QString dm_code = "-";
        QString gtin = "-";
        QString serialNumber = "-";
        int countryCode = 0;
        bool isAlcohol = false;
        QString country = "-";
        QString verificationKey = "-";
        QString verificationKeyValue = "-";
        bool isLongFormat = false;
    };

    enum class Country {
        Unknown = 0,
        Armenia = 1,
        Belarus = 2,
        Kazakhstan = 3,
        Kyrgyzstan = 4,
        Russia = 5
    };

    //---Enums
    static QString countryToString(Country country) {
        switch (country) {
        case Country::Armenia: return "Республика Армения";
        case Country::Belarus: return "Республика Белоруссия";
        case Country::Kazakhstan: return "Республика Казахстан";
        case Country::Kyrgyzstan: return "Киргизская Республика";
        case Country::Russia: return "Российская Федерация";
        default: return "Неизвестная страна";
        }
    }


    //---Funcs
    static DataMatrixAttrs parseDataMatrix(const QString &dataMatrix);
    static bool validateDataMatrix(const QString &dataMatrix, DataMatrixAttrs& attrs);
    static QPixmap generateDataMatrix(const QString& data, int size);
    static QString prepareDataMatrix(const QString &code);

private:
    Ui::DMInfoForm *ui;

    //---Funcs
    void setDMFields(const DataMatrixAttrs &attrs);
    static QString normalizeGS(QString input);

};

#endif // DMINFOFORM_H
