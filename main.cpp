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

    gSettings.loadSettings("/ini/settings.ini");
    styleManager.setTheme(StyleManager::LightStyle);

    MainWindow w;
    w.show();
    return a.exec();
}
