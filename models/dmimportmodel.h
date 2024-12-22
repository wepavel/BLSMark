#ifndef DMIMPORTMODEL_H
#define DMIMPORTMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class DMImportModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        // Number,
        // Code,
        // Page,
        // DataMatrix,
        NumPageColumn,
        CodeColumn,
        ImgColumn,
        ColumnCount
    };

    explicit DMImportModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addRow(const QString &code, int page, const QString &imgBase64);
    void clear();

private:
    struct RowData {
        int page;
        QString code;
        QString imgBase64;
    };

    QVector<RowData> m_data;
};


#endif // DMIMPORTMODEL_H
