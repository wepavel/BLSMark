// #ifndef AUTOSCROLLTABLEVIEW_H
// #define AUTOSCROLLTABLEVIEW_H

// #include "qevent.h"
// #include <QTableView>
// #include <QScrollBar>
// #include <QTimer>

// class AutoScrollTableView : public QTableView
// {
//     Q_OBJECT

// public:
//     AutoScrollTableView(QWidget *parent = nullptr) : QTableView(parent), autoScrollEnabled(true)
//     {
//         // Отслеживание прокрутки
//         connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &AutoScrollTableView::onScroll);
//     }

//     void addData()
//     {
//         if (autoScrollEnabled) {
//             scrollToBottom();  // Прокрутка вниз после добавления данных
//         }
//     }

// protected:
//     void wheelEvent(QWheelEvent *event) override
//     {
//         // Перехватим колесико мыши, чтобы отследить, проматывает ли пользователь вручную
//         if (event->angleDelta().y() < 0) {
//             // Пользователь прокручивает вниз
//             autoScrollEnabled = false;
//             //qDebug() << "autoScrollEnabled FALSE" ;
//             onScroll(verticalScrollBar()->value());
//         } else if (event->angleDelta().y() > 0) {
//             // Пользователь прокручивает вверх
//             autoScrollEnabled = false;
//             //qDebug() << "autoScrollEnabled FALSE" ;
//         }

//         QTableView::wheelEvent(event);
//     }

// private slots:
//     void onScroll(int value)
//     {
//         // Проверяем, если пользователь достиг конца таблицы
//         int maxValue = verticalScrollBar()->maximum();
//         if (value == maxValue) {
//             autoScrollEnabled = true;  // Разрешаем прокрутку вниз, если внизу
//             //qDebug() << "autoScrollEnabled TRUE" ;
//         }
//     }

//     void scrollToBottom()
//     {
//         // Прокрутить до последней строки
//         verticalScrollBar()->setValue(verticalScrollBar()->maximum());
//     }

// private:
//     bool autoScrollEnabled;  // Флаг, чтобы проверить, нужно ли прокручивать вниз
// };

// #endif

#ifndef AUTOSCROLLTABLEVIEW_H
#define AUTOSCROLLTABLEVIEW_H

#include <QTableView>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>

class AutoScrollTableView : public QTableView
{
    Q_OBJECT

public:
    AutoScrollTableView(QWidget *parent = nullptr) : QTableView(parent), autoScrollEnabled(true)
    {
        connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &AutoScrollTableView::onScroll);

        scrollTimer = new QTimer(this);
        scrollTimer->setSingleShot(true);
        connect(scrollTimer, &QTimer::timeout, this, &AutoScrollTableView::performScrollToBottom);
    }

    void addData()
    {
        if (autoScrollEnabled) {
            scrollTimer->start(0);
        }
    }

protected:
    void wheelEvent(QWheelEvent *event) override
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

private slots:
    void onScroll(int value)
    {
        int maxValue = verticalScrollBar()->maximum();
        if (value == maxValue) {
            autoScrollEnabled = true;
        } else {
            autoScrollEnabled = false;
        }
    }

    void performScrollToBottom()
    {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }

private:
    bool autoScrollEnabled;
    QTimer *scrollTimer;
};

#endif
