#include "autoscrolltableview.h"

AutoScrollTableView::AutoScrollTableView(QWidget *parent)
    : QTableView(parent), autoScrollEnabled(true)
{
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &AutoScrollTableView::onScroll);

    scrollTimer = new QTimer(this);
    scrollTimer->setSingleShot(true);
    connect(scrollTimer, &QTimer::timeout, this, &AutoScrollTableView::performScrollToBottom);
}

void AutoScrollTableView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0) {
        // Пользователь прокручивает вниз
        QTableView::wheelEvent(event);
        onScroll(verticalScrollBar()->value());
    } else if (event->angleDelta().y() > 0) {
        // Пользователь прокручивает вверх
        autoScrollEnabled = false;
        QTableView::wheelEvent(event);
    }
}

void AutoScrollTableView::onScroll(int value)
{
    int maxValue = verticalScrollBar()->maximum();
    if (value == maxValue) {
        autoScrollEnabled = true;
    } else {
        autoScrollEnabled = false;
    }
}

void AutoScrollTableView::performScrollToBottom()
{
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void AutoScrollTableView::update()
{
    if (autoScrollEnabled) {
        scrollTimer->start(0);
    }
}
