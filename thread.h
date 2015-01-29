/*
 * Copyright 2014 Victor Manuel Fernandez Castro
 *
 * This file is part of Mandelbrot.
 *
 * Mandelbrot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mandelbrot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef THREAD_H
#define THREAD_H

#include <QtGui>

class Widget;
class QImage;

class Thread : public QThread
{
    Q_OBJECT

public:
    explicit Thread(Widget *widget);
    bool event(QEvent *event);

    void setPalette(int num);

    inline static void setParallelize(bool value)

    {
        parallelize = value;
    }

    static void mandelbrot(QImage *image, const QRect &fragment, const QPointF &offset, double diff);

private:
    Widget *widget;
    QVector<QRgb> palette;
    QMutex mutexPalette;
    static bool parallelize;
};

#endif // THREAD_H
