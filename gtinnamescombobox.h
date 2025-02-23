#ifndef GTINNAMESCOMBOBOX_H
#define GTINNAMESCOMBOBOX_H

#include "core/httpmanager.h"
#include <QComboBox>

class GtinNamesComboBox : public QComboBox
{
public:
    GtinNamesComboBox(QWidget* parent = nullptr);
    void updateNames();
    QString getGtin() const;

private:
    HttpManager* httpManager;

    void getAllGtinsNamesSlot(const QByteArray& responseData, int statusCode);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // GTINNAMESCOMBOBOX_H
