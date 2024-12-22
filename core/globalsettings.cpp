#include "globalsettings.h"
#include <QApplication>
#include <QFile>

GlobalSettings& GlobalSettings::instance()
{
    static GlobalSettings instance;
    return instance;
}

void GlobalSettings::load_settings(const QString& filename)
{
    m_filename = QString("%1%2")
                     .arg(app_path)
                     .arg(filename);
    m_settings = new QSettings(m_filename, QSettings::IniFormat);

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

void GlobalSettings::save_settings()
{
    m_settings->sync();
}

QVariant GlobalSettings::get_value(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    m_settings->beginGroup(group);
    QVariant value = m_settings->value(key, defaultValue);
    m_settings->endGroup();
    return value;
}

void GlobalSettings::set_value(const QString &group, const QString &key, const QVariant &value)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
}

QString GlobalSettings::get_app_path() const
{
    return app_path;
}


