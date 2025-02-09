#ifndef DMIMPORTMODEL_H
#define DMIMPORTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QTableView>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>

class DMImportModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        // Number,
        // Code,
        // Page,
        // DataMatrix,
        IdColumn,
        CodeColumn,
        FilenameColumn,
        ImgColumn,
        ColumnCount
    };

    explicit DMImportModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addRow(const QString& gtin,
                const QString &code,
                const QString &filename,
                const QString &imgBase64);
    void clear();
    QSet<QString> getAllGtins() const;
    QList<QString> getAllDmCodes() const;

signals:
    void dataHasBeenAdded();
    void dataHasBeenCleared();

private:
    struct RowData {
        QString gtin;
        QString code;
        QString filename;
        QString imgBase64;
    };

    QVector<RowData> m_data;
};



// class DmImportTableView : public QTableView
// {
//     Q_OBJECT

// public:
//     DmImportTableView(QWidget *parent = nullptr) : QTableView(parent)
//     {
//         viewport()->installEventFilter(this);
//     }

// protected:
//     bool eventFilter(QObject *watched, QEvent *event) override
//     {
//         if (watched == viewport() && event->type() == QEvent::ToolTip)
//         {
//             QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
//             QModelIndex index = indexAt(helpEvent->pos());

//             if (!index.isValid())
//             {
//                 return true; // Блокируем событие tooltip для невалидных индексов
//             }

//             // Для валидных индексов показываем tooltip
//             QToolTip::showText(helpEvent->globalPos(),
//                                model()->data(index, Qt::ToolTipRole).toString(),
//                                this,
//                                visualRect(index));
//             return true;
//         }
//         return QTableView::eventFilter(watched, event);
//     }

//     void leaveEvent(QEvent *event) override
//     {
//         QToolTip::hideText(); // Скрываем tooltip при выходе из области таблицы
//         QTableView::leaveEvent(event);
//     }
// };
#endif // DMIMPORTMODEL_H



