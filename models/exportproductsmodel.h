#ifndef EXPORTPRODUCTSMODEL_H
#define EXPORTPRODUCTSMODEL_H

#include <QAbstractTableModel>

class ExportProductsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ProductNameColumn,
        CodeColumn,
        ColumnCount
    };

    explicit ExportProductsModel(QObject *parent = nullptr);

private:
    struct RowData {
        QString productName;
        QString code;
    };

    QVector<RowData> m_data;

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool isEmpty() const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addRow(const QString &code,
                const QString &productName);
    QPair<bool, QString> saveToCsv(const QString& fullPath);
    void clear();
};

#endif // EXPORTPRODUCTSMODEL_H
