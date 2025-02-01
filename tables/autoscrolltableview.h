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
    AutoScrollTableView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onScroll(int value);
    void performScrollToBottom();

public slots:
    void update();

private:
    bool autoScrollEnabled;
    QTimer *scrollTimer;
};

#endif
