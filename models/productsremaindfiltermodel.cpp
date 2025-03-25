#include "productsremaindfiltermodel.h"
#include "models/productsremaindmodel.h"

ProductsRemaindFilterModel::ProductsRemaindFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(ProductsRemaindModel::ProductNameColumn);
}

void ProductsRemaindFilterModel::setFilterProductName(const QString &filterString)
{
    m_filterString = filterString;
    invalidateFilter();
}

bool ProductsRemaindFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterString.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, ProductsRemaindModel::ProductNameColumn, sourceParent);

    QString productName = sourceModel()->data(index).toString();

    return productName.contains(m_filterString, Qt::CaseInsensitive);
}
