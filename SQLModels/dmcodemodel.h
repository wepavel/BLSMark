#ifndef DMCODEMODEL_H
#define DMCODEMODEL_H

#include <QStringList>
#include <QVariantMap>
#include <QDateTime>

#include "sqlmodels/baseentity.h"


class DMCodeModel : public BaseEntity
{
public:
    QString id;  // Изменено на QString для ULID
    QString code;
    QString img_base64;
    QDateTime import_date;

    DMCodeModel() : import_date(QDateTime::currentDateTime()) {}  // Конструктор по умолчанию

    QString tableName() const override { return "dmcodes"; }

    QStringList fields() const override {
        return {"id", "code", "img_base64", "import_date"};
    }

    QVariantMap toMap() const override {
        return {
            {"id", id},
            {"code", code},
            {"img_base64", img_base64},
            {"import_date", import_date}
        };
    }

    void fromMap(const QVariantMap& map) override {
        id = map["id"].toString();  // Изменено на toString() для ULID
        code = map["code"].toString();
        img_base64 = map["img_base64"].toString();
        import_date = map["import_date"].toDateTime();
    }
};

#endif // DMCODEMODEL_H
