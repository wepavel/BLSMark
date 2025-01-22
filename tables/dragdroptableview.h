#include <QTableView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

class DragDropTableView : public QTableView
{
    Q_OBJECT

public:
    DragDropTableView(QWidget* parent = nullptr) : QTableView(parent)
    {
        setAcceptDrops(true);
        parentWidget()->setAcceptDrops(true);
        setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
        setSelectionMode(SingleSelection);
    }

signals:
    void filesWereDropped(const QStringList& filePaths, const QStringList& dirs);

private:
    int m_currentRow = -1;  // Хранит индекс текущей выделенной строки

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override {
        //QTableView::dragMoveEvent(event);
    };
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;

    // void mouseMoveEvent(QMouseEvent *event) override;
    // void leaveEvent(QEvent *event) override;
    // void wheelEvent(QWheelEvent *event);
};
