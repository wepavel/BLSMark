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

    StyleManager::ThemeType getTheme() const;
    void setTheme(StyleManager::ThemeType theme);

    QString getDataMatrixExtractPath() const;
    bool setDataMatrixExtractPath(const QString& path);

    QString getGsWin64Path() const;
    bool setGsWin64Path(const QString& path);

    QString getBackendServiceIP() const;
    void setBackendServiceIP(const QString& ip);

    int getBackendServicePort() const;
    void setBackendServicePort(const int port);

    QString getAppPath() const;
    QString getClientId();


signals:
    void backendServiceIpPortChanged();

private:
    GlobalSettings() {
        app_path = QCoreApplication::applicationDirPath();
    } // Приватный конструктор
    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    QVariant getValue(const QString &group, const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString &group, const QString& key, const QVariant& value);

    QString parsePath(QString path);
    bool checkRightPath(const QString& path);

    void initializeSettings();
    static QString generateClientId();

    //---Vars
    QSettings* m_settings;
    QString m_filename;
    QString app_path;

    StyleManager::ThemeType m_theme;
    QString m_dataMatrixExtractPath;
    QString m_gsWin64Path;
    QString m_backendServiceIP;
    int m_backendServicePort;
    QString client_ws_id = "";
};

// Глобальный макрос для удобного доступа
#define gSettings GlobalSettings::instance()

#endif // GLOBALSETTINGS_H
