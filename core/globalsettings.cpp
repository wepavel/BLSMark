#include "globalsettings.h"
#include <QApplication>
#include <QFile>

GlobalSettings& GlobalSettings::instance()
{
    static GlobalSettings instance;
    return instance;
}

void GlobalSettings::loadSettings(const QString& filename)
{
    m_filename = QString("%1%2")
                     .arg(app_path)
                     .arg(filename);
    qDebug() << m_filename;
    m_settings = new QSettings(m_filename, QSettings::IniFormat);
    initializeSettings();
    // QStringList groups = m_settings->childGroups();
    // // Проходим по всем группам
    // for (auto &group : std::as_const(groups)) {
    //     // Переходим в группу
    //     m_settings->beginGroup(group);

    //     // Получаем все ключи в текущей группе
    //     QStringList keys = m_settings->allKeys();
    //     // Проходим по всем ключам в группе
    //     for (const QString &key : keys)
    //     {
    //         QVariant value = m_settings->value(key);
    //         qDebug() << "  Key:" << key << "Value:" << value.toString();

    //     }

    //     // Выходим из группы
    //     m_settings->endGroup();
    // }



    // m_settings->sync();
    // m_settings->beginGroup("STYLE");
    // // style = string_to_theme(m_settings->value("mode").toString());
    // m_settings->endGroup();

    // // auto ret1 = m_settings->value("/THEME.mode").toString();

    // // set_theme(style);
    // auto x = 1;
}

void GlobalSettings::saveSettings()
{
    m_settings->sync();
}

StyleManager::ThemeType GlobalSettings::getTheme() const
{
    return m_theme;
}

void GlobalSettings::setTheme(StyleManager::ThemeType theme)
{
    setValue("STYLE", "mode", StyleManager::themeToString(theme));
    m_theme = theme;
}

QString GlobalSettings::getDataMatrixExtractPath() const
{
    return m_dataMatrixExtractPath;
}

bool GlobalSettings::setDataMatrixExtractPath(const QString &path)
{
    if(!checkRightPath(path))
        return false;
    setValue("PROGRAMS", "dm_parser", parsePath(path));
    m_dataMatrixExtractPath = path;
    return true;
}

QString GlobalSettings::getGsWin64Path() const
{
    return m_gsWin64Path;
}

bool GlobalSettings::setGsWin64Path(const QString &path)
{
    if(!checkRightPath(path))
        return false;
    setValue("PROGRAMS", "ghostscript", parsePath(path));
    m_gsWin64Path = path;
    return true;
}

QString GlobalSettings::getBackendServiceIP() const
{
    return m_backendServiceIP;
}

void GlobalSettings::setBackendServiceIP(const QString &ip)
{
    setValue("CONNECTIONS", "backend_service_ip", ip);
    m_backendServiceIP = ip;
}

int GlobalSettings::getBackendServicePort() const
{
    return m_backendServicePort;
}

void GlobalSettings::setBackendServicePort(const int port)
{
    setValue("CONNECTIONS", "backend_service_port", port);
    m_backendServicePort = port;
}

QVariant GlobalSettings::getValue(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    m_settings->beginGroup(group);
    QVariant value = m_settings->value(key, defaultValue);
    m_settings->endGroup();
    return value;
}

void GlobalSettings::setValue(const QString &group, const QString &key, const QVariant &value)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
}

QString GlobalSettings::parsePath(QString path)
{
    QString appPath = getAppPath();
    if(path.startsWith(appPath)) {
        if(QFile::exists(appPath))
            return path;
    } else if(path.startsWith("./")) {
        QString new_path = appPath + path.removeFirst();
        if(QFile::exists(new_path))
            return new_path;
    }
    // Если файл не найден
    return "Error: File not found!";
}

bool GlobalSettings::checkRightPath(const QString& path)
{
    QString res = parsePath(path);
    if(res == "Error: File not found!")
        return false;
    else
        return true;
}

void GlobalSettings::initializeSettings()
{
    m_theme = StyleManager::stringToTheme(getValue("STYLE", "mode", "LightStyle").toString());
    m_dataMatrixExtractPath = getValue("PROGRAMS", "dm_parser", "").toString();
    m_gsWin64Path = getValue("PROGRAMS", "ghostscript", "").toString();
    m_backendServiceIP = getValue("CONNECTIONS", "backend_service_ip", "").toString();
    m_backendServicePort = getValue("CONNECTIONS", "backend_service_port").toInt();
}

QString GlobalSettings::getAppPath() const
{
    return app_path;
}


