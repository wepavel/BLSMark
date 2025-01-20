//#include "precompiled.h"   // Precompiled-header with '#include <QxOrm.h>' and '#include "export.h"'
// #include "person.h"          // Class definition 'person'
// #include <QxOrm_Impl.h>     // Automatic memory leak detection and boost serialization export macro

// QX_REGISTER_CPP(person)   // This macro is necessary to register 'person' class in QxOrm context

// namespace qx {
// template <> void register_class(QxClass<person> & t)
// {
//     t.setName("t_person");               // 'person' C++ class is mapped to 't_person' database table

//     t.id(& person::id, "id");               // Register 'person::id' <=> primary key in your database
//     t.data(& person::firstName, "first_name");      // Register 'person::firstName' property mapped to 'first_name' database column name
//     t.data(& person::lastName, "last_name");  // Register 'person::lastName' property mapped to 'last_name' database column name
//     t.data(& person::birthDate, "birth_date");  // Register 'person::birthDate' property mapped to 'birth_date' database column name
// }}
