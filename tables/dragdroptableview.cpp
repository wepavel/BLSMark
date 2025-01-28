#include "dragdroptableview.h"


void DragDropTableView::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        bool isValidDrop = false;

        for (const QUrl& url : urls) {
            QString path = url.toLocalFile();
            QFileInfo fileInfo(path);

            if (fileInfo.suffix() == "eps" || fileInfo.suffix() == "pdf") { // если это нужный файл
                isValidDrop = true;
            } else if (fileInfo.isDir()) { // если это папка
                QDir dir(path);
                QStringList files = dir.entryList(QStringList() << "*.eps" << "*.pdf", QDir::Files);
                if (!files.isEmpty()) { // если в папке есть нужные файлы
                    isValidDrop = true;
                } else {
                    isValidDrop = false;
                }
            }
        }

        // if (isValidDrop) {
        //     setStyleSheet("QTableView { background-color: #D3FFD3; }");  // Зеленый цвет
        // } else {
        //     setStyleSheet("QTableView { background-color: #FFD3D3; }");  // Красный цвет
        // }
    }
    event->accept();
}

void DragDropTableView::dropEvent(QDropEvent *event)
{
    //qDebug() << "dropEvent";
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        QStringList resultFilesPaths = {}; // конечный набор путей к файлам
        QStringList resultDirs  = {}; // конечный набор путей к папкам
        for (const QUrl& url : urls) {
            QString path = url.toLocalFile();
            QFileInfo fileInfo(path);

            if (fileInfo.suffix() == "eps" || fileInfo.suffix() == "pdf") { // если мы перетащили просто файл
                qDebug() << "Перетащен файл:" << path;
                resultFilesPaths.append(path); // добавим его путь
                // Тут можно добавить логику для обработки файлов .eps или .pdf
            } else if (fileInfo.isDir()) { // если перетащили папку
                QDir dir(path);
                QStringList files = dir.entryList(QStringList() << "*.eps" << "*.pdf", QDir::Files); // ищем нужные файлы

                if (!files.isEmpty()) { // если нужные файлы в папке есть
                    qDebug() << "Папка содержит файлы .eps или .pdf:" << path;
                    resultDirs.append(path);
                } else { // если нужных файлов в папке нет
                    qDebug() << "Папка не содержит файлов .eps или .pdf:" << path;
                }
            }
        }
        if(!resultFilesPaths.isEmpty() || !resultDirs.isEmpty()) // если были получены какие-то файлы
            emit filesWereDropped(resultFilesPaths, resultDirs); // отправим их
    }
    // setStyleSheet("QTableView { background-color: transparent; }");
    event->accept();
}

void DragDropTableView::dragLeaveEvent(QDragLeaveEvent *event)
{
    // Сбросить подсветку, если курсор покидает область таблицы
    // setStyleSheet("QTableView { background-color: transparent; }");
    event->accept();
}

// void DragDropTableView::mouseMoveEvent(QMouseEvent *event)
// {
//     QModelIndex index = indexAt(event->pos());
//     if (index.isValid()) {
//         int row = index.row();
//         // Если выделена другая строка, очищаем выделение
//         if (row != m_currentRow) {
//             selectionModel()->clearSelection();
//             selectionModel()->select(model()->index(row, 0),
//                                      QItemSelectionModel::Select | QItemSelectionModel::Rows);
//             m_currentRow = row;  // Обновляем текущую выбранную строку
//         }
//     }
//     QTableView::mouseMoveEvent(event);
// }

// void DragDropTableView::leaveEvent(QEvent *event)
// {
//     selectionModel()->clearSelection();  // Убираем выделение при выходе из таблицы
//     m_currentRow = -1;  // Сбрасываем текущую строку
//     QTableView::leaveEvent(event);
// }

// void DragDropTableView::wheelEvent(QWheelEvent *event)
// {
//     QModelIndex index = indexAt(event->position().toPoint());
//     qDebug() << index;
//     if (index.isValid()) {
//         int row = index.row();
//         // Если выделена другая строка, очищаем выделение
//         if (row != m_currentRow) {
//             selectionModel()->clearSelection();
//             selectionModel()->select(model()->index(row, 0),
//                                      QItemSelectionModel::Select | QItemSelectionModel::Rows);
//             m_currentRow = row;  // Обновляем текущую выбранную строку
//         }
//     }
//     QTableView::wheelEvent(event);
// }
