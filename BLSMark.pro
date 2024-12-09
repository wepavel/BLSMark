QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# Определяем базовую директорию для выходных файлов
BASE_DIR = $$PWD/target

# Настройка для Debug и Release конфигураций
CONFIG(debug, debug|release) {
    DESTDIR = $$BASE_DIR/debug
} else {
    DESTDIR = $$BASE_DIR/release
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
