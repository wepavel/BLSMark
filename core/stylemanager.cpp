#include "stylemanager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

StyleManager& StyleManager::instance()
{
    static StyleManager instance;
    return instance;
}

auto StyleManager::setTheme(const ThemeType &theme) -> void
{
    QString style_path = "";
    QApplication *app = qobject_cast<QApplication*>(QApplication::instance());
    if (theme == ThemeType::DarkStyle) {
        style = ThemeType::DarkStyle;
        style_path = ":/styles/styles/qdarkstyle/style.qss";
    } else {
        style = ThemeType::LightStyle;
        style_path = ":/styles/styles/qlightstyle/style.qss";
    }
    QFile style_file(style_path);
    if(style_file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&style_file);
        QString styleSheet = textStream.readAll();
        style_file.close();
        app->setStyleSheet(styleSheet);
    } else {
        qDebug() << "Can't open stylesheet";
    }
}

auto StyleManager::toggleTheme() -> void
{
    qDebug() << "Change theme";
    if (style == ThemeType::DarkStyle) {
        setTheme(ThemeType::LightStyle);
    } else {
        setTheme(ThemeType::DarkStyle);
    }
}
