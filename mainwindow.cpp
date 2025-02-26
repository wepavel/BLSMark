#include "mainwindow.h"
#include "core/messager.h"
#include "qtconcurrentrun.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QLabel>
#include <QSizeGrip>
#include <QFile>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QWidgetAction>
#include "core/globalsettings.h"
#include "core/stylemanager.h"
#include "widgets/dmexportform.h"
#include "widgets/dmimportform.h"
#include "widgets/dminfoform.h"
#include "widgets/msgtoolbutton.h"
#include <widgets/connectionstateform.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tab_main->setFocusPolicy(Qt::NoFocus);
    ui->tab_export->setFocusPolicy(Qt::NoFocus);
    ui->tab_import->setFocusPolicy(Qt::NoFocus);
    setup_notifications_widgets();
    // qDebug() << "Основной поток " << QThread::currentThreadId();
    // QtConcurrent::run([](){
    //     Messager::instance().addMessage("Первое сообщение в другом потоке!");
    //     qDebug() << "Сообщение было создано в потоке: " << QThread::currentThreadId();
    // });

    setWindowTitle("BLS Mark");

    DMExportForm* csvExporter = new DMExportForm(this);
    ui->tab_export->layout()->addWidget(csvExporter);

    DMImportForm* pdfImporter = new DMImportForm(this);
    ui->tab_import->layout()->addWidget(pdfImporter);

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
    label->setText(tr("<a href='http://blscom.ru/' style='color: orange; font-family: Arial, sans-serif; font-size: 14px; text-decoration: underline;'>Посетите наш сайт http://blscom.ru</a>"));
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

void MainWindow::setup_notifications_widgets()
{
    MsgToolButton* e = new MsgToolButton(this);

    connect(&Messager::instance(), &Messager::msgWasAdded,
            e, &MsgToolButton::incrementMessageCount, Qt::QueuedConnection);

    connect(Messager::instance().getView(), &MsgLogWidget::dataHasBeenCleared,
            e, &MsgToolButton::clearAll, Qt::QueuedConnection);

    connect(e, &MsgToolButton::tb_clicked,
            Messager::instance().getView(), &MsgLogWidget::showWithOpenTab, Qt::QueuedConnection);

    ui->menubar->setCornerWidget(e, Qt::TopRightCorner);

    // Messager::instance().addMessage("msg", Error);
    // Messager::instance().addMessage("msg", Warning);
    // Messager::instance().addMessage("msg", Info, true);
}
