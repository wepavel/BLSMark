#ifndef GOODSMODEL_H
#define GOODSMODEL_H

#include <QAbstractTableModel>

class GoodsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        CodeColumn,
        GoodNameColumn,
        CountryColumn,
        TimeDbAdd,
        TimeStickOnGood,
        ColumnCount
    };

    explicit GoodsModel(QObject *parent = nullptr);

private:
    struct RowData {
        QString code;
        QString goodName;
        QString country;
        qint64 timeDbAdd;
        qint64 timeStickOnGood;
    };

    QVector<RowData> m_data;

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addRow(const QString &code,
                const QString &goodName,
                const QString &country,
                const qint64& timeDbAdd,
                const qint64& timeStickOnGood);
    void clear();

};

#endif // GOODSMODEL_H
