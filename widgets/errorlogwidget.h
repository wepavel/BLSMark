#ifndef ERRORLOGWIDGET_H
#define ERRORLOGWIDGET_H

#include <QWidget>

namespace Ui {
class ErrorLogWidget;
}

class ErrorLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorLogWidget(QWidget *parent = nullptr);
    ~ErrorLogWidget();

private slots:
    void on_pb_clear_clicked();

private:
    Ui::ErrorLogWidget *ui;
    void addMsg(const QString& msg);
};

#endif // ERRORLOGWIDGET_H
