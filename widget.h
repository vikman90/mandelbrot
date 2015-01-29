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

#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include "thread.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    bool event(QEvent *event);
    void setPalette(int num);

    inline int& getZoomKey()
    {
        return zoomKey;
    }

    inline int& getRightKey()
    {
        return rightKey;
    }

    inline int& getDownKey()
    {
        return downKey;
    }

    inline QImage& getImage()
    {
        return *image;
    }

signals:
    void statusMessage(const QString&);

public slots:
    void setPreview(bool activated);
    void readOptions();

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QImage *image;
    QPoint mouseCur;
    QPoint mousePrev;
    Thread *thread;
    QRectF surface;
    double diff;
    double zoomscale;
    int moveScale;
    int zoomKey;
    int rightKey;
    int downKey;
    int askCode;
    bool preview;
    bool parallelize;
    bool updated;
    int changes;
    int idTimer;

    void move(const QPoint &moving);
    void scale(double zoom);
    void askPaint();
};

#endif // WIDGET_H
