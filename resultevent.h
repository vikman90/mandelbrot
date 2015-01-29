#ifndef RESULTEVENT_H
#define RESULTEVENT_H

#include <QtCore>

class QImage;

class ResultEvent : public QEvent
{
public:
    explicit ResultEvent(int _code, QImage *_image) :
        QEvent(User), code(_code), image(_image) { }

    int code;
    QImage *image;
};

#endif // RESULTEVENT_H
