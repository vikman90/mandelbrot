#ifndef ASKEVENT_H
#define ASKEVENT_H

#include <QtCore>

class AskEvent : public QEvent
{
public:
    explicit AskEvent(int _code, const QRect &_window, const QPointF &_offset, double _diff) :
        QEvent(User), code(_code), window(_window), offset(_offset), diff(_diff) { }

    int code;
    QRect window;
    QPointF offset;
    double diff;
};

#endif // ASKEVENT_H
