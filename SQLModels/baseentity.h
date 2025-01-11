#ifndef BASEENTITY_H
#define BASEENTITY_H

#include <QString>
#include <QStringList>


class BaseEntity {
public:
    virtual ~BaseEntity() = default;
    virtual QString tableName() const = 0;
    virtual QStringList fields() const = 0;
    virtual QVariantMap toMap() const = 0;
    virtual void fromMap(const QVariantMap& map) = 0;
    virtual QStringList uniqueFields() const {
        return QStringList(); // По умолчанию возвращает пустой список
    }
};

#endif // BASEENTITY_H
