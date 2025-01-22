#include "statelabel.h"

StateLabel::StateLabel(QWidget *parent)
    : QLabel(parent)
{
    yesPix.load(":/images/img/yes.png");
    noPix.load(":/images/img/no.png");
    yesPix = yesPix.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    noPix = noPix.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(yesPix);
}

void StateLabel::setState(bool ok)
{
    if(ok)
        setPixmap(yesPix);
    else
        setPixmap(noPix);
}
