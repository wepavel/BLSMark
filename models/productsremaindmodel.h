#ifndef PRODUCTSREMAINDMODEL_H
#define PRODUCTSREMAINDMODEL_H

#include <QAbstractTableModel>

class ProductsRemaindModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ProductNameColumn,
        RemainderColumn,
        ColumnCount
    };

    explicit ProductsRemaindModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addRow(const QString &productName,
                const int &remainder);
    void clear();

private:
    struct RowData {
        QString productName;
        int remainder;
    };

    QVector<RowData> m_data;
};

#endif // PRODUCTSREMAINDMODEL_H
