#ifndef CRUDDMCODE_H
#define CRUDDMCODE_H

#include "crudbase.h"
#include <QString>

class CRUDDMCode : public CRUDBase
{
public:
    CRUDDMCode(QSqlDatabase* db = nullptr) : CRUDBase(db) {}

    CRUDDMCode(const QString& driver, const QString& dbName,
                 const QString& host = QString(), int port = -1,
                 const QString& user = QString(), const QString& password = QString())
        : CRUDBase(driver, dbName, host, port, user, password) {}

    template<typename T>
    std::unique_ptr<T> findByCode(const QString& code) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        query.prepare(QString("SELECT * FROM %1 WHERE code = :code").arg(T().tableName()));
        query.bindValue(":code", code);

        bool test = query.exec();

        if (!query.exec() || !query.next()) {
            qDebug() << "Find by code error:" << query.lastError().text();
            return nullptr;
        }

        auto entity = std::make_unique<T>();
        QVariantMap map;
        for (const auto& field : entity->fields()) {
            map[field] = query.value(field);
        }
        entity->fromMap(map);
        return entity;
    }

    template<typename T>
    QList<T> findAllByCode(const QString& code) {
        SpinLockGuard guard(m_lock);
        QList<T> result;
        QSqlQuery query(*m_db);
        query.prepare(QString("SELECT * FROM %1 WHERE code = :code").arg(T().tableName()));
        query.bindValue(":code", code);

        if (!query.exec()) {
            qDebug() << "Find all by code error:" << query.lastError().text();
            return result;
        }

        while (query.next()) {
            T entity;
            QVariantMap map;
            for (const auto& field : entity.fields()) {
                map[field] = query.value(field);
            }
            entity.fromMap(map);
            result.append(entity);
        }

        return result;
    }

    template<typename T>
    bool updateByCode(const QString& code, const T& entity) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        QStringList updateFields;
        for (const auto& field : entity.fields()) {
            if (field != "code") {
                updateFields << QString("%1 = :%1").arg(field);
            }
        }

        query.prepare(QString("UPDATE %1 SET %2 WHERE code = :code")
                          .arg(entity.tableName(), updateFields.join(", ")));

        auto map = entity.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }
        query.bindValue(":code", code);

        if (!query.exec()) {
            QSqlError error = query.lastError();
            if (error.type() == QSqlError::StatementError &&
                error.text().contains("UNIQUE constraint failed", Qt::CaseInsensitive)) {
                qDebug() << "Unique constraint violation:" << error.text();
            } else {
                qDebug() << "Update by code error:" << error.text();
            }
            return false;
        }
        return true;
    }

    template<typename T>
    bool deleteByCode(const QString& code) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        query.prepare(QString("DELETE FROM %1 WHERE code = :code").arg(T().tableName()));
        query.bindValue(":code", code);

        if (!query.exec()) {
            qDebug() << "Delete by code error:" << query.lastError().text();
            return false;
        }
        return true;
    }
};

#endif // CRUDDMCODE_H
