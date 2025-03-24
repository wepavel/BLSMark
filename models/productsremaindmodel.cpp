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

int ProductsRemaindModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
    // FIXME: Implement me!
}

int ProductsRemaindModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return Column::ColumnCount;
    // FIXME: Implement me!
}

bool ProductsRemaindModel::isEmpty() const
{
    return m_data.size()==0;
}

bool ProductsRemaindModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool ProductsRemaindModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void ProductsRemaindModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant ProductsRemaindModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool ProductsRemaindModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ProductsRemaindModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool ProductsRemaindModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool ProductsRemaindModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

void ProductsRemaindModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
