#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <qfiledialog.h>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    getDataFromIniFile();
    ui->pb_apply->setEnabled(false);
    ui->pb_open_dm_path->setFocusPolicy(Qt::NoFocus);
    ui->pb_open_gs_path->setFocusPolicy(Qt::NoFocus);
    initUIConnections();
}

void SettingsDialog::getDataFromIniFile()
{
    switch (gSettings.getTheme())
    {
    case StyleManager::LightStyle:
        ui->rb_light->setChecked(true);
        break;

    case StyleManager::DarkStyle:
        ui->rb_dark->setChecked(true);
        break;

    default:
        break;
    }
    ui->le_dm_path->setText(gSettings.getDataMatrixExtractPath());
    ui->le_gs_win64->setText(gSettings.getGsWin64Path());
    //qDebug() << gSettings.getBackendServiceIP();
    ui->le_backend_ser_ip->setText(gSettings.getBackendServiceIP());
    ui->le_backend_ser_port->setText(QString::number(gSettings.getBackendServicePort()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pb_apply_clicked()
{
    gSettings.setBackendServiceIP(ui->le_backend_ser_ip->text());
    gSettings.setBackendServicePort(ui->le_backend_ser_port->text().toInt());
    if(ui->rb_light->isChecked())
        gSettings.setTheme(StyleManager::LightStyle);
    else
        gSettings.setTheme(StyleManager::DarkStyle);
    gSettings.setDataMatrixExtractPath(ui->le_dm_path->text());
    gSettings.setGsWin64Path(ui->le_gs_win64->text());


    done(QDialog::Accepted);
}

void SettingsDialog::on_pb_cancel_clicked()
{
    done(QDialog::Rejected);
}


void SettingsDialog::on_pb_open_dm_path_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Выберите путь до программы считывания QR-кодов"),
                                                    QString(),
                                                    tr("Executable Files (*.exe);"));
    if (!filePath.isEmpty())
        ui->le_dm_path->setText(filePath);
}


void SettingsDialog::on_pb_open_gs_path_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Выберите путь до программы gs"),
                                                    QString(),
                                                    tr("Executable Files (*.exe);"));
    if (!filePath.isEmpty())
        ui->le_gs_win64->setText(filePath);
}

void SettingsDialog::initUIConnections()
{
    connect(ui->le_dm_path, &QLineEdit::textChanged, this, &SettingsDialog::setApplyButtonState);
    connect(ui->le_gs_win64, &QLineEdit::textChanged, this, &SettingsDialog::setApplyButtonState);
    connect(ui->le_backend_ser_ip, &QLineEdit::textChanged, this, &SettingsDialog::setApplyButtonState);
    connect(ui->le_backend_ser_port, &QLineEdit::textChanged, this, &SettingsDialog::setApplyButtonState);
    connect(ui->rb_dark, &QRadioButton::toggled, this, &SettingsDialog::setApplyButtonState);
    connect(ui->rb_light, &QRadioButton::toggled, this, &SettingsDialog::setApplyButtonState);
}

bool SettingsDialog::isSettingsChanged()
{
    if (ui->le_dm_path->text() != gSettings.getDataMatrixExtractPath() ||
        ui->le_gs_win64->text() != gSettings.getGsWin64Path() ||
        ui->le_backend_ser_ip->text() != gSettings.getBackendServiceIP() ||
        ui->le_backend_ser_port->text().toInt() != gSettings.getBackendServicePort() ||
        ui->rb_dark->isChecked() != (gSettings.getTheme() == StyleManager::DarkStyle) ||
        ui->rb_light->isChecked() != (gSettings.getTheme() == StyleManager::LightStyle)) {
        return true;
    }
    return false;
}

void SettingsDialog::setApplyButtonState()
{
    ui->pb_apply->setEnabled(isSettingsChanged());
}

