#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QMetaEnum>

class StyleManager: public QObject
{
    Q_OBJECT
public:
    static StyleManager& instance();

    //---Structs
    enum ThemeType
    {
        DarkStyle = 0,
        LightStyle = 1,
        CustomStyle = 10
    };
    Q_ENUM(ThemeType)

    //---Funcs
    void toggleTheme();
    void setTheme(const ThemeType &theme);

    static QString themeToString(ThemeType theme)
    {
        const QMetaEnum metaEnum = QMetaEnum::fromType<ThemeType>();
        return QString(metaEnum.valueToKey(theme));
    }

    static ThemeType stringToTheme(const QString& themeString)
    {
        const QMetaEnum metaEnum = QMetaEnum::fromType<ThemeType>();
        bool ok;
        int value = metaEnum.keyToValue(themeString.toUtf8().constData(), &ok);
        return ok ? static_cast<ThemeType>(value) : DarkStyle; // По умолчанию возвращаем DarkStyle
    }

private:
    //---Funcs
    StyleManager() {};
    ~StyleManager() {};

    //---Vars
    ThemeType style = ThemeType::LightStyle;
};

// Глобальный макрос для удобного доступа
#define styleManager StyleManager::instance()

#endif // STYLEMANAGER_H
