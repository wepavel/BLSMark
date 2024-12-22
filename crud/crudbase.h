#ifndef CRUDBASE_H
#define CRUDBASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QAtomicInt>
#include <optional>
#include <memory>


class CRUDBase
{
public:
    // Конструктор с существующей QSqlDatabase или создание по умолчанию
    CRUDBase(QSqlDatabase* db = nullptr)
        : m_ownDb(false)
        , m_lock(0)
    {
        if (db) {
            m_db = db;
        } else {
            m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
            m_db->setDatabaseName("mydatabase.db");
            if (!m_db->open()) {
                qDebug() << "Error opening database:" << m_db->lastError().text();
            }
            m_ownDb = true;
        }
    }

    // Конструктор с ручным указанием параметров QSqlDatabase
    CRUDBase(const QString& driver, const QString& dbName,
             const QString& host = QString(), int port = -1,
             const QString& user = QString(), const QString& password = QString())
        : m_ownDb(true)
        , m_lock(0)
    {
        m_db = new QSqlDatabase(QSqlDatabase::addDatabase(driver));
        m_db->setDatabaseName(dbName);

        if (!host.isEmpty())
            m_db->setHostName(host);
        if (port != -1)
            m_db->setPort(port);
        if (!user.isEmpty())
            m_db->setUserName(user);
        if (!password.isEmpty())
            m_db->setPassword(password);

        if (!m_db->open()) {
            qDebug() << "Error opening database:" << m_db->lastError().text();
        }
    }

    ~CRUDBase() {
        if (m_ownDb) {
            if (m_db->isOpen()) {
                m_db->close();
            }
            delete m_db;
        }
    }

    template<typename T>
    bool insert(const T& entity) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        QString fields = entity.fields().join(", ");
        QString placeholders = QString(":%1").arg(entity.fields().join(", :"));

        query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)")
                          .arg(entity.tableName(), fields, placeholders));

        auto map = entity.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }

        if (!query.exec()) {
            qDebug() << "Insert error:" << query.lastError().text();
            return false;
        }
        return true;
    }

    template<typename T>
    QList<T> findAll() {
        SpinLockGuard guard(m_lock);
        QList<T> result;
        QSqlQuery query(*m_db);
        query.prepare(QString("SELECT * FROM %1").arg(T().tableName()));

        if (!query.exec()) {
            qDebug() << "Find all error:" << query.lastError().text();
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

    // Остальные методы (findById, update, deleteById) аналогично обернуть в SpinLockGuard

private:
    //---Vars
    QSqlDatabase* m_db;
    bool m_ownDb;
    QAtomicInt m_lock;

    //---Funcs
    QString getSqlType(QVariant::Type type) {
        switch (type) {
        case QVariant::Int:
            return "INTEGER";
        case QVariant::Double:
            return "REAL";
        case QVariant::String:
            return "TEXT";
        case QVariant::Bool:
            return "BOOLEAN";
        case QVariant::DateTime:
            return "DATETIME";
        case QVariant::ByteArray:
            return "BLOB";
        default:
            return "TEXT";
        }
    }

    //---Classes
    class SpinLockGuard {
    public:
        SpinLockGuard(QAtomicInt& lock) : m_lock(lock) {
            while (m_lock.fetchAndStoreAcquire(1) != 0) {
                // Спин-ожидание
            }
        }
        ~SpinLockGuard() {
            m_lock.storeRelease(0);
        }
    private:
        QAtomicInt& m_lock;
    };
};

#endif // CRUDBASE_H
