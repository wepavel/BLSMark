#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


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


private:
    //---Vars
    Ui::MainWindow *ui;
    QString style = "dark";


    //---Funcs
    void fill_logo();
    void toggle_theme();
};
#endif // MAINWINDOW_H
