#include "unloadgoodsmodel.h"

#include "core/messager.h"
#include "qbrush.h"
#include "qdatetime.h"

#include <qfile.h>

UnloadGoodsModel::UnloadGoodsModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

int UnloadGoodsModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

int UnloadGoodsModel::columnCount(const QModelIndex &parent) const
{
    return Column::ColumnCount;
}

bool UnloadGoodsModel::isEmpty() const
{
    return m_data.size()==0;
}

QVariant UnloadGoodsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const RowData &rowData = m_data[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::CodeColumn: return rowData.code;
        case Column::ProductNameColumn: return rowData.goodName;
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // Выравнивание по центру
    }

    return QVariant();
}

QVariant UnloadGoodsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {
        case Column::CodeColumn: return tr("Код");
        case Column::ProductNameColumn: return tr("Название товара");
        default: return QVariant();
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags UnloadGoodsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

void UnloadGoodsModel::addRow(const QString &code,
                        const QString &goodName)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append({goodName, code});
    endInsertRows();
}

QPair<bool, QString> UnloadGoodsModel::saveToCsv(const QString &fullPath)
{
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        messagerInst.addMessage("Не удалось открыть файл для записи: "+fullPath, Error);
        return QPair<bool, QString>(false, "Не удалось открыть файл для записи:"+fullPath);
    }

    QTextStream out(&file);

    for (const RowData &row : m_data) {
        out << row.code << "\n";
    }

    file.close();

    return QPair<bool, QString>(true,"");
}

void UnloadGoodsModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

