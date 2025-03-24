#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "core/messager.h"
#include "dialogs/settingsdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void windowClosed(); // Пользовательский сигнал, который вызывается при закрытии

protected:
    void closeEvent(QCloseEvent *event) override {
        // connect(this, &MainWindow::closeEvent,
        //         &Messager::instance(), &Messager::destroyUI, Qt::QueuedConnection);

        // connect(this, &MainWindow::, this, [this](){
        //     auto x = 1;
        //     auto y = 2;
        //     qDebug() << "Close Event working";
        // });
        Messager::instance().destroyUI();
        event->accept(); // Разрешаем закрытие окна
    }

private slots:
    void on_act_open_product_from_triggered();

private:
    //---Vars
    Ui::MainWindow *ui;
    QString style = "dark";


    //---Funcs
    void fillLogo();
    void toggleTheme();
    void setupMenubar();
    void setupNotificationsWidgets();
    void checkLicense();

};
#endif // MAINWINDOW_H
