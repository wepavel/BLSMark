#include "autoscrolllistwidget.h"

AutoScrollListWidget::AutoScrollListWidget(QWidget *parent)
    : QListWidget(parent), autoScrollEnabled(true)
{
    // Подключаем сигнал изменения значения скроллбара к слоту onScroll
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &AutoScrollListWidget::onScroll);

    // Настраиваем таймер для автоматической прокрутки
    scrollTimer = new QTimer(this);
    scrollTimer->setSingleShot(true);
    connect(scrollTimer, &QTimer::timeout, this, &AutoScrollListWidget::performScrollToBottom);
}

void AutoScrollListWidget::update()
{
    if (autoScrollEnabled) {
        scrollTimer->start(0); // Прокручиваем вниз
    }
}

// Переопределяем метод добавления элемента
void AutoScrollListWidget::addItem(const QString &itm)
{
    QListWidget::addItem(itm);
    update();
}

void AutoScrollListWidget::addItem(QListWidgetItem *itm)
{
    QListWidget::addItem(itm);
    update();
}

void AutoScrollListWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0) {
        // Пользователь прокручивает вниз
        QListWidget::wheelEvent(event);
        onScroll(verticalScrollBar()->value());
    } else if (event->angleDelta().y() > 0) {
        // Пользователь прокручивает вверх
        autoScrollEnabled = false;
        QListWidget::wheelEvent(event);
    }
}

void AutoScrollListWidget::onScroll(int value)
{
    int maxValue = verticalScrollBar()->maximum();
    if (value == maxValue) {
        // Если пользователь долистал до конца, включаем автоматическую прокрутку
        autoScrollEnabled = true;
    } else {
        // Иначе отключаем автоматическую прокрутку
        autoScrollEnabled = false;
    }
}

void AutoScrollListWidget::performScrollToBottom()
{
    // Прокручиваем до конца
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}
