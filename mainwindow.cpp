#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QLabel>
#include <QSizeGrip>
#include <QFile>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include "core/globalsettings.h"
#include "core/stylemanager.h"
#include "widgets/dmimportform.h"
#include "widgets/dminfoform.h"
#include <widgets/connectionstateform.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("BLS Mark");



    DMImportForm* pdf_importer = new DMImportForm(this);
    ui->tab_import->layout()->addWidget(pdf_importer);

    ConnectionStateForm* conStateForm = new ConnectionStateForm(this);
    ui->tab_main->layout()->addWidget(conStateForm);

    setup_menubar();
    fill_logo();
    statusBar()->showMessage(tr("Offline mode"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

auto MainWindow::fill_logo() -> void
{
    QPixmap mark_pixmap(":/images/img/mark_full.png");

    QLabel *mark_label = new QLabel();
    mark_label->setPixmap(mark_pixmap);
    mark_label->setScaledContents(true);
    mark_label->setFixedSize(150, 60); // Установите нужный размер
    ui->hl_logo->addWidget(mark_label);

    ui->hl_logo->addStretch(1);

    QLabel *label = new QLabel(this);
    label->setText(tr("<a href='http://blscom.ru/'>Посетите наш сайт</a>"));
    label->setOpenExternalLinks(true); // Это позволит открывать ссылку в браузере по умолчанию

    // Если вы хотите обрабатывать клик самостоятельно:
    label->setOpenExternalLinks(false);
    connect(label, &QLabel::linkActivated, [](const QString &link) {
        QDesktopServices::openUrl(QUrl(link));
    });
    ui->hl_logo->addWidget(label);
    ui->hl_logo->addStretch(1);

    QPixmap bls_pixmap(":/images/img/bls_logo.png");
    QLabel *image_label = new QLabel();
    image_label->setPixmap(bls_pixmap);
    image_label->setScaledContents(true);
    image_label->setFixedSize(120, 60); // Установите нужный размер

    ui->hl_logo->addWidget(image_label);

}

void MainWindow::toggle_theme()
{
    styleManager.toggleTheme();
}

void MainWindow::setup_menubar()
{
    connect(ui->act_check_code, &QAction::triggered, this, [this](){
        bool ok;
        QString text = QInputDialog::getText(this, tr("Проверка кода"),
                                             tr("Введите код:"), QLineEdit::Normal,
                                             "", &ok);

        qDebug() << "TEEEST";

        if (ok && !text.isEmpty()) {
            QDialog dialog;

            QWidget *widget = new DMInfoForm(text, &dialog);


            QVBoxLayout *layout = new QVBoxLayout(&dialog);
            layout->addWidget(widget);
            dialog.setLayout(layout);

            // Настройка вашего виджета

            int result = dialog.exec();
        } else return;

    });
    connect(ui->act_settings, &QAction::triggered, this, [this](){
        SettingsDialog d(this);
        d.exec();
    });
}
