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
        // SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);

        QStringList fieldsList = entity.fields();
        fieldsList.removeAll("id");  // Удаляем 'id' из списка полей

        QString fields = fieldsList.join(", ");
        QString placeholders = QString(":%1").arg(fieldsList.join(", :"));

        query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)")
                          .arg(entity.tableName(), fields, placeholders));

        auto map = entity.toMap();
        for (const auto& field : fieldsList) {
            query.bindValue(":" + field, map[field]);
        }

        if (!query.exec()) {
            QSqlError error = query.lastError();
            if (error.type() == QSqlError::StatementError &&
                error.text().contains("UNIQUE constraint failed", Qt::CaseInsensitive)) {
                qDebug() << "Unique constraint violation:" << error.text();
            } else {
                qDebug() << "Insert error:" << error.text();
            }
            return false;
        }

        // Получаем сгенерированный id и обновляем объект
        int lastInsertId = query.lastInsertId().toInt();
        const_cast<T&>(entity).m_id = lastInsertId;

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

    template<typename T>
    std::unique_ptr<T> findById(int id) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        query.prepare(QString("SELECT * FROM %1 WHERE id = :id").arg(T().tableName()));
        query.bindValue(":id", id);

        if (!query.exec() || !query.next()) {
            qDebug() << "Find by ID error:" << query.lastError().text();
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
    bool update(const T& entity) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        QStringList updateFields;
        for (const auto& field : entity.fields()) {
            if (field != "id") {
                updateFields << QString("%1 = :%1").arg(field);
            }
        }

        query.prepare(QString("UPDATE %1 SET %2 WHERE id = :id")
                          .arg(entity.tableName(), updateFields.join(", ")));

        auto map = entity.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }

        if (!query.exec()) {
            QSqlError error = query.lastError();
            if (error.type() == QSqlError::StatementError &&
                error.text().contains("UNIQUE constraint failed", Qt::CaseInsensitive)) {
                qDebug() << "Unique constraint violation:" << error.text();
            } else {
                qDebug() << "Update error:" << error.text();
            }
            return false;
        }
        return true;
    }

    template<typename T>
    bool deleteById(int id) {
        SpinLockGuard guard(m_lock);
        QSqlQuery query(*m_db);
        query.prepare(QString("DELETE FROM %1 WHERE id = :id").arg(T().tableName()));
        query.bindValue(":id", id);

        if (!query.exec()) {
            qDebug() << "Delete error:" << query.lastError().text();
            return false;
        }
        return true;
    }

    template<typename T>
    bool createTable() {
        SpinLockGuard guard(m_lock);
        T tempEntity;
        QString tableName = tempEntity.tableName();

        QStringList columnDefs;
        for (const auto& field : tempEntity.fields()) {
            QString columnType;
            if (field == "id") {
                columnDefs << "id INTEGER PRIMARY KEY AUTOINCREMENT";
            } else {
                // Здесь вы можете добавить логику для определения типа столбца
                // На основе имени поля или дополнительной информации из вашей модели
                columnType = "TEXT";  // По умолчанию используем TEXT
                columnDefs << QString("%1 %2").arg(field, columnType);
            }
        }

        // Добавляем ограничения уникальности
        QStringList uniqueFields = tempEntity.uniqueFields();
        for (const auto& field : uniqueFields) {
            columnDefs << QString("UNIQUE(%1)").arg(field);
        }

        QString createTableQuery = QString("CREATE TABLE IF NOT EXISTS %1 (%2)")
                                       .arg(tableName, columnDefs.join(", "));

        qDebug() << "Create table query:" << createTableQuery;  // Добавьте эту строку для отладки

        QSqlQuery query(*m_db);
        if (!query.exec(createTableQuery)) {
            qDebug() << "Error creating table:" << query.lastError().text();
            return false;
        }
        return true;
    }

private:
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

protected:
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

    QAtomicInt m_lock;
    //---Vars
    QSqlDatabase* m_db;
    bool m_ownDb;
};

#endif // CRUDBASE_H

