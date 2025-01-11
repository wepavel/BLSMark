#ifndef DOUBLEPROGRESSDIALOG_H
#define DOUBLEPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class DoubleProgressDialog;
}

class DoubleProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoubleProgressDialog(QWidget *parent = nullptr);
    ~DoubleProgressDialog();


    void setFileProgress(quint8 progress);
    quint8 getFileProgress();

    void setFilesProgress(quint8 progress);
    quint8 getFilesProgress();


private:
    Ui::DoubleProgressDialog *ui;
};

#endif // DOUBLEPROGRESSDIALOG_H
