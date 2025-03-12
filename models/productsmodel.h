#ifndef PRODUCTSMODEL_H
#define PRODUCTSMODEL_H

#include <QAbstractTableModel>

class ProductsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        CodeColumn,
        ProductNameColumn,
        CountryColumn,
        TimeDbAdd,
        TimeStickOnProduct,
        ColumnCount
    };

    explicit ProductsModel(QObject *parent = nullptr);

private:
    struct RowData {
        QString code;
        QString productName;
        QString country;
        qint64 timeDbAdd;
        qint64 timeStickOnProduct;
    };

    QVector<RowData> m_data;

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addRow(const QString &code,
                const QString &productName,
                const QString &country,
                const qint64 &timeDbAdd,
                const qint64 &timeStickOnProduct);
    void clear();

signals:
    void dataHasBeenAdded();
};

#endif // PRODUCTSMODEL_H
