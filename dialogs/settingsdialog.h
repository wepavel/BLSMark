#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "core/globalsettings.h"
#include "core/stylemanager.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    void getDataFromIniFile();
    ~SettingsDialog();

private slots:
    void on_pb_apply_clicked();

    void on_pb_cancel_clicked();

    void on_pb_open_dm_path_clicked();

    void on_pb_open_gs_path_clicked();

private:
    Ui::SettingsDialog *ui;

    void initUIConnections();
    bool isSettingsChanged();
    void setApplyButtonState();
};

#endif // SETTINGSDIALOG_H
