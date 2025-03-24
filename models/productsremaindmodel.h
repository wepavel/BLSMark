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

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool isEmpty() const;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    void clear();

private:
    struct RowData {
        QString productName;
        int remainder;
    };

    QVector<RowData> m_data;
};

#endif // PRODUCTSREMAINDMODEL_H
