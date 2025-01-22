#ifndef STATELABEL_H
#define STATELABEL_H

#include "qlabel.h"
#include "qpixmap.h"
#include <QObject>

class StateLabel : public QLabel
{
public:
    StateLabel(QWidget *parent = nullptr);

    void setState(bool ok);

private:
    QPixmap yesPix;
    QPixmap noPix;
};

#endif // STATELABEL_H
