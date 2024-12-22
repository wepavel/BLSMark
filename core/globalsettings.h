#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QMetaEnum>
#include <QCoreApplication>


class GlobalSettings: public QObject
{
    Q_OBJECT
public:
    static GlobalSettings& instance();

    void load_settings(const QString& filename = "config.ini");
    void save_settings();

    QVariant get_value(const QString &group, const QString& key, const QVariant& defaultValue = QVariant()) const;
    void set_value(const QString &group, const QString& key, const QVariant& value);
    QString get_app_path() const;

private:
    //---Funcs
    GlobalSettings() {
        app_path = QCoreApplication::applicationDirPath();
    } // Приватный конструктор
    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    QSettings* m_settings;
    QString m_filename;
    QString app_path;
};

// Глобальный макрос для удобного доступа
#define gSettings GlobalSettings::instance()

#endif // GLOBALSETTINGS_H
