#include <QTableView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

class DragDropTableView : public QTableView
{
    Q_OBJECT

public:
    DragDropTableView(QWidget* parent = nullptr) : QTableView(parent)
    {
        setAcceptDrops(true);
        parentWidget()->setAcceptDrops(true);
    }

signals:
    void filesWereDropped(const QStringList& filePaths);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override
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

            if (isValidDrop) {
                setStyleSheet("QTableView { background-color: #D3FFD3; }");  // Зеленый цвет
            } else {
                setStyleSheet("QTableView { background-color: #FFD3D3; }");  // Красный цвет
            }
        }
        event->accept();
    }

    void dragMoveEvent(QDragMoveEvent *event){
        //QTableView::dragMoveEvent(event);
    };

    void dropEvent(QDropEvent* event) override
    {
        //qDebug() << "dropEvent";
        const QMimeData* mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            QList<QUrl> urls = mimeData->urls();
            QStringList resultFilesPaths = {}; // конечный набор путей к файлам
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
                        // Добавляем путь до папки ко всем файлам в списке
                        for (int i = 0; i < files.size(); ++i) {
                            files[i].prepend(path+"/"); // собираем их пути
                            resultFilesPaths.append(files[i]); // добавляем им для передачи
                        }
                    } else { // если нужных файлов в папке нет
                        qDebug() << "Папка не содержит файлов .eps или .pdf:" << path;
                    }
                }
            }
            if(!resultFilesPaths.isEmpty()) // если были получены какие-то файлы
                emit filesWereDropped(resultFilesPaths); // отправим их
        }
        setStyleSheet("QTableView { background-color: transparent; }");
        event->accept();
    }

    void dragLeaveEvent(QDragLeaveEvent* event) override
    {
        // Сбросить подсветку, если курсор покидает область таблицы
        setStyleSheet("QTableView { background-color: transparent; }");
        event->accept();
    }
};
