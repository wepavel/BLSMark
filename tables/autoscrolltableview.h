#ifndef AUTOSCROLLTABLEVIEW_H
#define AUTOSCROLLTABLEVIEW_H

#include "qevent.h"
#include <QTableView>
#include <QScrollBar>
#include <QTimer>

class AutoScrollTableView : public QTableView
{
    Q_OBJECT

public:
    AutoScrollTableView(QWidget *parent = nullptr) : QTableView(parent), autoScrollEnabled(true)
    {
        // Отслеживание прокрутки
        connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &AutoScrollTableView::onScroll);
    }

    void addData()
    {
        if (autoScrollEnabled) {
            scrollToBottom();  // Прокрутка вниз после добавления данных
        }
    }

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        // Перехватим колесико мыши, чтобы отследить, проматывает ли пользователь вручную
        if (event->angleDelta().y() < 0) {
            // Пользователь прокручивает вниз
            autoScrollEnabled = false;
            //qDebug() << "autoScrollEnabled FALSE" ;
            onScroll(verticalScrollBar()->value());
        } else if (event->angleDelta().y() > 0) {
            // Пользователь прокручивает вверх
            autoScrollEnabled = false;
            //qDebug() << "autoScrollEnabled FALSE" ;
        }

        QTableView::wheelEvent(event);
    }

private slots:
    void onScroll(int value)
    {
        // Проверяем, если пользователь достиг конца таблицы
        int maxValue = verticalScrollBar()->maximum();
        if (value == maxValue) {
            autoScrollEnabled = true;  // Разрешаем прокрутку вниз, если внизу
            //qDebug() << "autoScrollEnabled TRUE" ;
        }
    }

    void scrollToBottom()
    {
        // Прокрутить до последней строки
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }

private:
    bool autoScrollEnabled;  // Флаг, чтобы проверить, нужно ли прокручивать вниз
};

#endif
