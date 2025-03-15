#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H

#include <QString>

#define SALT

class LicenseManager
{
public:
    LicenseManager();

    //---Funcs
    static QString getMotherboardSerial();
    static QString createAugmentedHash(const QString& input, const QString &salt);
    static void writeHashToRegistry(const QString& hash);
    static QString readHashFromRegistry();
    static QString readHashFromRegistryWithValidation(const QString& serial, const QString& salt);

    static bool checkLicense(const QString &salt);
    static bool removeFromRegistry(const QString& key, const QString& valueName = QString());

    static QString getDefaultSalt() {
        return "KIPiA_2025";
    }

};

#endif // LICENSEMANAGER_H
