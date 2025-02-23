#ifndef UNLOADGOODSMODEL_H
#define UNLOADGOODSMODEL_H

#include <QAbstractTableModel>

class UnloadGoodsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ProductNameColumn,
        CodeColumn,
        ColumnCount
    };

    explicit UnloadGoodsModel(QObject *parent = nullptr);

private:
    struct RowData {
        QString goodName;
        QString code;
    };

    QVector<RowData> m_data;

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addRow(const QString &code,
                const QString &goodName);
    void clear();
};

#endif // UNLOADGOODSMODEL_H
