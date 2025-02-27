#ifndef EXPORTGOODSMODEL_H
#define EXPORTGOODSMODEL_H

#include <QAbstractTableModel>

class ExportGoodsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ProductNameColumn,
        CodeColumn,
        ColumnCount
    };

    explicit ExportGoodsModel(QObject *parent = nullptr);

private:
    struct RowData {
        QString goodName;
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
                const QString &goodName);
    QPair<bool, QString> saveToCsv(const QString& fullPath);
    void clear();
};

#endif // EXPORTGOODSMODEL_H
