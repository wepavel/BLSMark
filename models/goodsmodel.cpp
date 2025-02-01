#include "goodsmodel.h"
#include "qbrush.h"
#include "qdatetime.h"

GoodsModel::GoodsModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

int GoodsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int GoodsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(Column::ColumnCount);
}

QVariant GoodsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const RowData &rowData = m_data[index.row()];

    if (role == Qt::BackgroundRole) {
        if (m_data[index.row()].timeStickOnGood == 0) {
            return QBrush(Qt::red); // Установим красный цвет фона
        }
    }

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::CodeColumn: return rowData.code;
        case Column::GoodNameColumn: return rowData.goodName;
        case Column::CountryColumn: return rowData.country;

        case Column::TimeDbAdd: {
            return QDateTime::fromSecsSinceEpoch(rowData.timeDbAdd)
                .toString("dd.MM.yyyy HH:mm:ss");
        }

        case Column::TimeStickOnGood: {
            if(rowData.timeStickOnGood == 0){
                return "";
            }
            return QDateTime::fromSecsSinceEpoch(rowData.timeStickOnGood)
                .toString("dd.MM.yyyy HH:mm:ss");
        }

        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // Выравнивание по центру
    }

    return QVariant();
}

QVariant GoodsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::CodeColumn: return tr("Код");
        case Column::GoodNameColumn: return tr("Название");
        case Column::CountryColumn: return tr("Страна производства");
        case Column::TimeDbAdd: return tr("Время добавления в БД");
        case Column::TimeStickOnGood: return tr("Время наклейки QR-кода");
        default: return QVariant();
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags GoodsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

void GoodsModel::addRow(const QString &code,
                        const QString &goodName,
                        const QString &country,
                        const qint64 &timeDbAdd,
                        const qint64 &timeStickOnGood)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append({code, goodName, country, timeDbAdd, timeStickOnGood});
    endInsertRows();
    emit dataHasBeenAdded();
}

void GoodsModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
