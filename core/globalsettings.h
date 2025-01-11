#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QMetaEnum>
#include <QCoreApplication>

#include "stylemanager.h"


class GlobalSettings: public QObject
{
    Q_OBJECT
public:
    static GlobalSettings& instance();

    void loadSettings(const QString& filename = "config.ini");
    void saveSettings();

    void getTheme();
    void setTheme(StyleManager::ThemeType theme);

    QString getAppPath() const;
    QString getGhostScriptPath() const;
    QString getBSLDMParserPath() const;

private:
    //---Funcs
    GlobalSettings() {
        app_path = QCoreApplication::applicationDirPath();
    } // Приватный конструктор
    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    QVariant getValue(const QString &group, const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString &group, const QString& key, const QVariant& value);

    QString parsePath(QString path);

    //---Vars
    QSettings* m_settings;
    QString m_filename;
    QString app_path;
};

// Глобальный макрос для удобного доступа
#define gSettings GlobalSettings::instance()

#endif // GLOBALSETTINGS_H
