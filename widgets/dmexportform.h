#ifndef DMEXPORTFORM_H
#define DMEXPORTFORM_H

#include <QWidget>

namespace Ui {
class DMExportForm;
}

class DMExportForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMExportForm(QWidget *parent = nullptr);
    ~DMExportForm();

private slots:
    void on_pb_calendar_clicked();

private:
    Ui::DMExportForm *ui;
};

#endif // DMEXPORTFORM_H
