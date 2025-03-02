#ifndef AUTOSCROLLLISTWIDGET_H
#define AUTOSCROLLLISTWIDGET_H

#include <QListWidget>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <QDebug> // Для отладки

class AutoScrollListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit AutoScrollListWidget(QWidget *parent = nullptr);

    void addItem(QListWidgetItem *itm);
    void addItem(const QString& itm);


    void update();

protected:
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onScroll(int value);
    void performScrollToBottom();

private:
    bool autoScrollEnabled;
    QTimer *scrollTimer;
};

#endif // AUTOSCROLLLISTWIDGET_H
