#include "mainwindow.h"

#include <QApplication>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

#include "core/globalsettings.h"
#include "core/stylemanager.h"
#include "core/licensemanager.h"
// #include "widgets/msglogwidget.h"
#include "widgets/healthcheckform.h"
#include "core/wsmanager.h"
#include "widgets/msglogwidget.h"
#include "xlsxdocument.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gSettings.loadSettings("/ini/settings.ini");
    styleManager.setTheme(gSettings.getTheme());
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
