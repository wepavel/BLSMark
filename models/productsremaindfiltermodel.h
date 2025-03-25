#ifndef PRODUCTSREMAINDFILTERMODEL_H
#define PRODUCTSREMAINDFILTERMODEL_H

#include <QSortFilterProxyModel>

class ProductsRemaindFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProductsRemaindFilterModel(QObject *parent = nullptr);

    void setFilterProductName(const QString &filterString);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_filterString;
};

#endif // PRODUCTSREMAINDFILTERMODEL_H
