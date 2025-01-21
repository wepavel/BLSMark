QT += core gui concurrent sql


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
    core/globalsettings.cpp \
    core/stylemanager.cpp \
    dialogs/doubleprogressdialog.cpp \
    dialogs/settingsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    models/dmimportmodel.cpp \
    widgets/dmimportform.cpp \
    widgets/dminfoform.cpp

HEADERS += \
    crud/crudbase.h \
    crud/cruddmcode.h \
    dialogs/doubleprogressdialog.h \
    dialogs/settingsdialog.h \
    sqlmodels/baseentity.h \
    sqlmodels/dmcodemodel.h \
    core/globalsettings.h \
    core/stylemanager.h \
    mainwindow.h \
    models/dmimportmodel.h \
    widgets/dmimportform.h \
    widgets/dminfoform.h

FORMS += \
    dialogs/doubleprogressdialog.ui \
    dialogs/settingsdialog.ui \
    mainwindow.ui \
    widgets/dmimportform.ui \
    widgets/dminfoform.ui

RESOURCES += \
    resources.qrc \

DISTFILES += $$BASE_DIR/debug/ini/settings.ini

win32: RC_ICONS = ico/bls_ico.ico


# Путь к директории с библиотекой$
LIBS += -L$$PWD/side_packs/libs/dmtx -ldmtx
# Зависимость от статической библиотеки
PRE_TARGETDEPS += $$PWD/side_packs/libs/dmtx/libdmtx.a
INCLUDEPATH += $$PWD/side_packs/libs/dmtx


# D:\Qt\6.8.1\mingw_64\bin\lupdate.exe BLSMark.pro
TRANSLATIONS += bls_mark_ru.ts

