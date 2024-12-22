#ifndef BASEENTITY_H
#define BASEENTITY_H

#include <QString>


class BaseEntity {
public:
    virtual ~BaseEntity() = default;
    virtual QString tableName() const = 0;
    virtual QStringList fields() const = 0;
    virtual QVariantMap toMap() const = 0;
    virtual void fromMap(const QVariantMap& map) = 0;
};

#endif // BASEENTITY_H
