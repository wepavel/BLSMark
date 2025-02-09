#ifndef NAMEINPUTDIALOG_H
#define NAMEINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class NameInputDialog;
}

class NameInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameInputDialog(QWidget *parent = nullptr);
    QString getName() const;
    ~NameInputDialog();

private slots:
    void on_pb_add_clicked();
    void on_pb_cancel_clicked();

private:
    Ui::NameInputDialog *ui;
    QString name;
};

#endif // NAMEINPUTDIALOG_H
