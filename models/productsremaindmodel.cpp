#include "productsremaindmodel.h"

ProductsRemaindModel::ProductsRemaindModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

QVariant ProductsRemaindModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::ProductNameColumn: return tr("Наименование");
        case Column::RemainderColumn: return tr("Остаток");
        default: return QVariant();
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags ProductsRemaindModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;  // Невалидный индекс — никаких действий
    }

    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    return defaultFlags;
}

void ProductsRemaindModel::addRow(const QString &productName, const int &remainder)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append({productName, remainder});
    endInsertRows();
}

int ProductsRemaindModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int ProductsRemaindModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return Column::ColumnCount;
}

QVariant ProductsRemaindModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const RowData &rowData = m_data[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::ProductNameColumn: return rowData.productName;
        case Column::RemainderColumn: return rowData.remainder;
        default: return QVariant();
        }
    }

    return QVariant();
}

void ProductsRemaindModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
