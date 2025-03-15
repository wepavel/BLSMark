#include "licensemanager.h"

#include <QProcess>
#include <QCryptographicHash>
#include <QSettings>

LicenseManager::LicenseManager() {}

//-----------------------PRIVATE-----------------------
// Функция для получения серийного номера материнской платы
QString LicenseManager::getMotherboardSerial() {
    QProcess process;
    process.start("wmic", QStringList() << "baseboard" << "get" << "serialnumber");
    process.waitForFinished(-1);

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);

    if (lines.size() >= 2) {
        QString serial = lines[1].trimmed();
        if (!serial.isEmpty() && serial != "To be filled by O.E.M.") {
            return serial;
        }
    }

    // Если серийный номер недоступен, используем UUID системы
    process.start("wmic", QStringList() << "csproduct" << "get" << "uuid");
    process.waitForFinished(-1);

    output = process.readAllStandardOutput();
    lines = output.split("\n", Qt::SkipEmptyParts);

    if (lines.size() >= 2) {
        return lines[1].trimmed();
    }

    return QString();
}

// Функция для создания аугментированного хеша
QString LicenseManager::createAugmentedHash(const QString& input, const QString &salt) {
    // Здесь вы можете добавить свою собственную логику аугментации
    QString augmentedInput = input + salt;

    return QCryptographicHash::hash(augmentedInput.toUtf8(), QCryptographicHash::Sha256).toHex();
}

// Функция для записи хеша в реестр
// void LicenseManager::writeHashToRegistry(const QString& hash) {
//     QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
//                        QSettings::NativeFormat);
//     settings.setValue("LicenseHash", hash);
// }

// // Функция для чтения хеша из реестра
// QString LicenseManager::readHashFromRegistry() {
//     QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
//                        QSettings::NativeFormat);
//     return settings.value("LicenseHash").toString();
// }
void LicenseManager::writeHashToRegistry(const QString& hash) {
    auto writeTo = [&](QSettings& settings) {
        settings.setValue("LicenseHash", hash);
    };

    QSettings settings64("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark", QSettings::NativeFormat);
    QSettings settings32("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\BLSEngineering\\BLSMark", QSettings::NativeFormat);

    writeTo(settings64);
    writeTo(settings32);
}

QString LicenseManager::readHashFromRegistry() {
    auto readFrom = [&](QSettings& settings) -> QString {
        return settings.value("LicenseHash").toString();
    };

    QSettings settings64("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark", QSettings::NativeFormat);
    QSettings settings32("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\BLSEngineering\\BLSMark", QSettings::NativeFormat);

    QString hash64 = readFrom(settings64);
    if (!hash64.isEmpty()) {
        return hash64; // Приоритет значения из 64-битной ветки
    }

    return readFrom(settings32); // Если в 64-битной ветке ничего нет
}

QString LicenseManager::readHashFromRegistryWithValidation(const QString& serial, const QString& salt) {
    auto readFrom = [&](QSettings& settings) -> QString {
        return settings.value("LicenseHash").toString();
    };

    auto validateHash = [&](const QString& hash) -> bool {
        QString expectedHash = createAugmentedHash(serial, salt);
        return hash == expectedHash;
    };

    QSettings settings64("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark", QSettings::NativeFormat);
    QSettings settings32("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\BLSEngineering\\BLSMark", QSettings::NativeFormat);

    QString hash64 = readFrom(settings64);
    if (!hash64.isEmpty() && validateHash(hash64)) {
        return hash64; // Если хэш в 64-битной ветке корректен, возвращаем его
    }

    QString hash32 = readFrom(settings32);
    if (!hash32.isEmpty() && validateHash(hash32)) {
        return hash32; // Если хэш в 32-битной ветке корректен, возвращаем его
    }

    return QString(); // Если корректный хэш не найден
}


bool LicenseManager::checkLicense(const QString &salt) {
    QString serial = getMotherboardSerial();
    QString currentHash = createAugmentedHash(serial, salt);
    // QString storedHash = readHashFromRegistry();
    QString storedHash = readHashFromRegistryWithValidation(serial, salt);

    return (currentHash == storedHash);
}

// bool LicenseManager::removeFromRegistry(const QString& key, const QString& valueName)
// {
//     // Открываем ключ реестра
//     QSettings settings64("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
//                        QSettings::NativeFormat);

//     QSettings settings32("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\BLSEngineering\\BLSMark",
//                        QSettings::NativeFormat);


//     if (valueName.isEmpty()) {
//         // Если имя значения не указано, удаляем весь ключ
//         settings.remove(key);
//     } else {
//         // Если имя значения указано, удаляем конкретное значение
//         settings.beginGroup(key);
//         settings.remove(valueName);
//         settings.endGroup();
//     }

//     // Проверяем, было ли удаление успешным
//     if (valueName.isEmpty()) {
//         return !settings.contains(key);
//     } else {
//         settings.beginGroup(key);
//         bool result = !settings.contains(valueName);
//         settings.endGroup();
//         return result;
//     }
// }
bool LicenseManager::removeFromRegistry(const QString& key, const QString& valueName) {
    auto removeFrom = [&](QSettings& settings) -> bool {
        if (valueName.isEmpty()) {
            settings.remove(key);
            return !settings.contains(key);
        } else {
            settings.beginGroup(key);
            settings.remove(valueName);
            bool result = !settings.contains(valueName);
            settings.endGroup();
            return result;
        }
    };

    QSettings settings64("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark", QSettings::NativeFormat);
    QSettings settings32("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\BLSEngineering\\BLSMark", QSettings::NativeFormat);

    bool result64 = removeFrom(settings64);
    bool result32 = removeFrom(settings32);

    return result64 || result32;
}

