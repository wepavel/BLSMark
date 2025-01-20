#ifndef _DATABASE_VERSION_H_
#define _DATABASE_VERSION_H_

#include <QxOrm.h>
#include <QString>

class MY_DLL_EXPORT DatabaseVersion
{
public:
    QString name;
    long version;
};

QX_REGISTER_HPP_MY_APP(DatabaseVersion, qx::trait::no_base_class_defined, 0)

#endif // _DATABASE_VERSION_H_
