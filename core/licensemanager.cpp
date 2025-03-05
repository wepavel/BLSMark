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
void LicenseManager::writeHashToRegistry(const QString& hash) {
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
                       QSettings::NativeFormat);
    settings.setValue("LicenseHash", hash);
}

// Функция для чтения хеша из реестра
QString LicenseManager::readHashFromRegistry() {
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
                       QSettings::NativeFormat);
    return settings.value("LicenseHash").toString();
}

bool LicenseManager::checkLicense(const QString &salt) {
    QString serial = getMotherboardSerial();
    QString currentHash = createAugmentedHash(serial, salt);
    QString storedHash = readHashFromRegistry();

    return (currentHash == storedHash);
}

bool LicenseManager::removeFromRegistry(const QString& key, const QString& valueName)
{
    // Открываем ключ реестра
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\BLSEngineering\\BLSMark",
                       QSettings::NativeFormat);

    if (valueName.isEmpty()) {
        // Если имя значения не указано, удаляем весь ключ
        settings.remove(key);
    } else {
        // Если имя значения указано, удаляем конкретное значение
        settings.beginGroup(key);
        settings.remove(valueName);
        settings.endGroup();
    }

    // Проверяем, было ли удаление успешным
    if (valueName.isEmpty()) {
        return !settings.contains(key);
    } else {
        settings.beginGroup(key);
        bool result = !settings.contains(valueName);
        settings.endGroup();
        return result;
    }
}
