#include "mainwindow.h"

#include <QApplication>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include "core/globalsettings.h"
#include "core/stylemanager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gSettings.load_settings("/ini/settings.ini");
    styleManager.set_theme(StyleManager::LightStyle);

    MainWindow w;
    w.show();
    return a.exec();
}
