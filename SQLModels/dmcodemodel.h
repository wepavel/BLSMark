#ifndef DMCODEMODEL_H
#define DMCODEMODEL_H

#include <QStringList>
#include <QVariantMap>
#include <QDateTime>
#include <QObject>

#include "sqlmodels/baseentity.h"


class DMCodeModel : public QObject, public BaseEntity
{
    Q_OBJECT
public:
    explicit DMCodeModel(QObject *parent = nullptr)
        : QObject(parent)
        , m_id(0)
        , m_import_date(QDateTime::currentDateTime())
    {}

    explicit DMCodeModel(const QString &code, const QString &img_path = QString(), QObject *parent = nullptr)
        : QObject(parent)
        , m_id(0)
        , m_code(code)
        , m_img_path(img_path)
        , m_import_date(QDateTime::currentDateTime())
    {}

    int m_id;
    QString m_code;
    QString m_img_path;
    QDateTime m_import_date;

    // DMCodeModel() : m_import_date(QDateTime::currentDateTime()) {}  // Конструктор по умолчанию

    QString tableName() const override { return "dmcodes"; }

    QStringList fields() const override {
        return {"id", "code", "img_path", "import_date"};
    }

    QVariantMap toMap() const override {
        return {
            {"id", m_id},
            {"code", m_code},
            {"img_path", m_img_path},
            {"import_date", m_import_date}
        };
    }

    void fromMap(const QVariantMap& map) override {
        m_id = map["id"].toInt();
        m_code = map["code"].toString();
        m_img_path = map["img_path"].toString();
        m_import_date = map["import_date"].toDateTime();
    }

    QStringList uniqueFields() const override {
        return {"code", "img_path"};
    }
};

#endif // DMCODEMODEL_H
