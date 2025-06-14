QT += core gui concurrent sql network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG -= debug_and_release debug_and_release_target



# Убираем сообщения о deprecated
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

# Определяем базовую директорию для выходных файлов
BASE_DIR = $$PWD/target

# Настройка для Debug и Release конфигураций
CONFIG(release, debug|release) {
    DESTDIR = $$BASE_DIR/release
} else {
    DESTDIR = $$BASE_DIR/debug
}

# Устанавливаем выходную директорию для промежуточных файлов
OBJECTS_DIR = $$BASE_DIR/.obj
MOC_DIR = $$BASE_DIR/.moc
RCC_DIR = $$BASE_DIR/.rcc
UI_DIR = $$BASE_DIR/.ui

# Настройка post-build для Release конфигурации
CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$DESTDIR/$$TARGET.exe
}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/dmcodewshandler.cpp \
    core/globalsettings.cpp \
    core/httpmanager.cpp \
    core/licensemanager.cpp \
    core/messager.cpp \
    core/stylemanager.cpp \
    core/wsmanager.cpp \
    dialogs/doubleprogressdialog.cpp \
    dialogs/settingsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    models/dmimportmodel.cpp \
    models/exportproductsmodel.cpp \
    models/productsmodel.cpp \
    models/productsremaindfiltermodel.cpp \
    models/productsremaindmodel.cpp \
    tables/autoscrolllistwidget.cpp \
    tables/autoscrolltableview.cpp \
    tables/dragdroptableview.cpp \
    ui_components/gtinnamescombobox.cpp \
    ui_components/productsdatetimeedit.cpp \
    widgets/connectionstateform.cpp \
    widgets/controlpanelform.cpp \
    widgets/dmexportform.cpp \
    widgets/dmimportform.cpp \
    widgets/dminfoform.cpp \
    widgets/healthcheckform.cpp \
    widgets/healthunitform.cpp \
    widgets/msglogwidget.cpp \
    widgets/msgtoolbutton.cpp \
    widgets/productsform.cpp \
    widgets/statelabel.cpp

HEADERS += \
    core/dmcodewshandler.h \
    core/httpmanager.h \
    core/licensemanager.h \
    core/messager.h \
    core/wsmanager.h \
    crud/crudbase.h \
    crud/cruddmcode.h \
    dialogs/doubleprogressdialog.h \
    dialogs/settingsdialog.h \
    models/exportproductsmodel.h \
    models/productsmodel.h \
    models/productsremaindfiltermodel.h \
    models/productsremaindmodel.h \
    sqlmodels/baseentity.h \
    sqlmodels/dmcodemodel.h \
    core/globalsettings.h \
    core/stylemanager.h \
    mainwindow.h \
    models/dmimportmodel.h \
    tables/autoscrolllistwidget.h \
    tables/autoscrolltableview.h \
    tables/dragdroptableview.h \
    ui_components/gtinnamescombobox.h \
    ui_components/productsdatetimeedit.h \
    widgets/connectionstateform.h \
    widgets/controlpanelform.h \
    widgets/dmexportform.h \
    widgets/dmimportform.h \
    widgets/dminfoform.h \
    widgets/healthcheckform.h \
    widgets/healthunitform.h \
    widgets/msglogwidget.h \
    widgets/msgtoolbutton.h \
    widgets/productsform.h \
    widgets/statelabel.h

FORMS += \
    dialogs/doubleprogressdialog.ui \
    dialogs/settingsdialog.ui \
    mainwindow.ui \
    widgets/connectionstateform.ui \
    widgets/controlpanelform.ui \
    widgets/dmexportform.ui \
    widgets/dmimportform.ui \
    widgets/dminfoform.ui \
    widgets/healthcheckform.ui \
    widgets/healthunitform.ui \
    widgets/msglogwidget.ui \
    widgets/msgtoolbutton.ui \
    widgets/productsform.ui

RESOURCES += \
    resources.qrc \

DISTFILES += $$BASE_DIR/debug/ini/settings.ini

win32: RC_ICONS = ico/bls_ico.ico


# Путь к директории с библиотекой$
# LIBS += -L$$PWD/side_packs/libs/dmtx -ldmtx
# Зависимость от статической библиотеки
# PRE_TARGETDEPS += $$PWD/side_packs/libs/dmtx/libdmtx.a
INCLUDEPATH += $$PWD/side_packs/libs/dmtx
# QXlsx code for Application Qt project
QXLSX_PARENTPATH=$$PWD/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=$$PWD/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=$$PWD/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include($$PWD/QXlsx/QXlsx/QXlsx.pri)
# INCLUDEPATH += $$PWD/include/qxlsx

# Папка, где лежат скомпилированные библиотеки
LIBS += -L$$PWD/libs

# Подключаем библиотеки
# LIBS += -lQXlsx
LIBS += -ldmtx
# INCLUDEPATH += $$PWD/libs/include

# D:\Qt\6.8.1\mingw_64\bin\lupdate.exe BLSMark.pro
TRANSLATIONS += bls_mark_ru.ts

