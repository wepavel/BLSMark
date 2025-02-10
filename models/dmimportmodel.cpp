#include "dmimportmodel.h"

DMImportModel::DMImportModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

QVariant DMImportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<Column>(section)) {

        // case Column::Number: return tr("№");
        // case Column::Code: return tr("C");
        // case Column::Page: return tr("Страница");
            case Column::GTINColumn: return tr("GTIN");
            case Column::CodeColumn: return tr("Код");
            case Column::FilenameColumn: return tr("Имя файла");
            case Column::ImgColumn: return tr("Base64");
            default: return QVariant();
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section+1;
    }
    return QVariant();
}

int DMImportModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int DMImportModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(Column::ColumnCount);
}

QVariant DMImportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const RowData &rowData = m_data[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::GTINColumn: return rowData.gtin;
        case Column::CodeColumn: return rowData.code;
        case Column::FilenameColumn: return rowData.filename;
        case Column::ImgColumn: return "Image";
        // case Column::Number: return index.row() + 1;
        // case Column::Code: return rowData.code;
        // case Column::Page: return rowData.page;
        // case Column::DataMatrix: return rowData.dataMatrix;
        default: return QVariant();
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        switch (static_cast<Column>(index.column())) {
        case Column::GTINColumn:
            return QString("ID товара %1").arg(rowData.gtin);
        case Column::CodeColumn:
            return QString("Code: %1").arg(rowData.code);
        case Column::FilenameColumn:
            return QString("Filename: %1").arg(rowData.filename);
        case Column::ImgColumn:
            return QString("Base64 Image (Size: %1 bytes)").arg(rowData.imgBase64.size());
        // case Column::Number:
        //     return QString("Row number: %1").arg(index.row() + 1);
        // case Column::Code:
        //     return QString("Code: %1").arg(rowData.code);
        // case Column::Page:
        //     return QString("Page: %1").arg(rowData.page);
        // case Column::DataMatrix:
        //     return QString("DataMatrix: %1").arg(rowData.dataMatrix);
        default:
            return QVariant();
        }
    }

    return QVariant();
}

// bool DMImportModel::setData(const QModelIndex &index, const QVariant &value, int role)
// {
//     if (role != Qt::EditRole || !index.isValid())
//         return false;

//     RowData &rowData = m_data[index.row()];
//     switch (static_cast<Column>(index.column())) {
//     case Column::CodeColumn:
//         rowData.code = value.toString();
//         break;
//     case Column::FilenameColumn:
//         rowData.filename = value.toString();
//         break;
//     case Column::ImgColumn:
//         rowData.imgBase64 = value.toString();
//         break;
//     default:
//         return false;
//     }

//     emit dataChanged(index, index, {role});
//     return true;
// }

Qt::ItemFlags DMImportModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index); // убрал тут | Qt::ItemIsEditable
}

void DMImportModel::addRow(const QString& gtin,
                           const QString &code,
                           const QString &filename,
                           const QString &imgBase64)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append({gtin, code, filename, imgBase64});
    m_gtins.insert(gtin);
    emit dataHasBeenAdded();
    endInsertRows();
}

void DMImportModel::clear()
{
    beginResetModel();
    m_data.clear();
    m_gtins.clear();
    emit dataHasBeenCleared();
    endResetModel();
}

QSet<QString> DMImportModel::getAllGtins() const
{
    return m_gtins;
}

QList<QString> DMImportModel::getAllDmCodes() const
{
    QList<QString> allDmCodes;
    for(const RowData& data: m_data){
        allDmCodes.append(data.code);
    }
    return allDmCodes;
}
