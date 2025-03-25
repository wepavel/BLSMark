#include "productsmodel.h"
#include "qbrush.h"
#include "qdatetime.h"

ProductsModel::ProductsModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

int ProductsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int ProductsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return Column::ColumnCount;
}

QVariant ProductsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const RowData &rowData = m_data[index.row()];

    if (role == Qt::BackgroundRole) {
        if (m_data[index.row()].timeStickOnProduct == 0) {
            return QBrush(Qt::red); // Установим красный цвет фона
        }
    }

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::CodeColumn: return rowData.code;
        case Column::ProductNameColumn: return rowData.productName;
        case Column::CountryColumn: return rowData.country;

        case Column::TimeDbAdd: {
            return QDateTime::fromSecsSinceEpoch(rowData.timeDbAdd)
                .toString("dd.MM.yyyy HH:mm:ss");
        }

        case Column::TimeStickOnProduct: {
            if(rowData.timeStickOnProduct == 0){
                return "";
            }
            return QDateTime::fromSecsSinceEpoch(rowData.timeStickOnProduct)
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

QVariant ProductsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::CodeColumn: return tr("Код");
        case Column::ProductNameColumn: return tr("Название");
        case Column::CountryColumn: return tr("Страна производства");
        case Column::TimeDbAdd: return tr("Время добавления в БД");
        case Column::TimeStickOnProduct: return tr("Время наклейки QR-кода");
        default: return QVariant();
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags ProductsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

void ProductsModel::addRow(const QString &code,
                        const QString &productName,
                        const QString &country,
                        const qint64 &timeDbAdd,
                        const qint64 &timeStickOnProduct)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append({code, productName, country, timeDbAdd, timeStickOnProduct});
    endInsertRows();
    emit dataHasBeenAdded();
}

void ProductsModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
