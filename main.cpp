#include "mainwindow.h"

#include <QApplication>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include "core/globalsettings.h"
#include "core/stylemanager.h"
#include "widgets/errorlogwidget.h"
#include "widgets/healthcheckform.h"
#include <core/healthchecker.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gSettings.loadSettings("/ini/settings.ini");
    styleManager.setTheme(gSettings.getTheme());
    MainWindow w;
    w.show();

    ErrorLogWidget w1;
    w1.show();

    return a.exec();
}
