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

#include "widget.h"
#include <complex>
#include <QtWidgets>
#include "mainwindow.h"
#include "askevent.h"
#include "resultevent.h"
#include "options.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    thread = new Thread(this);
    image = new QImage();
    zoomKey = 0;
    rightKey = 0;
    downKey = 0;
    askCode = 0;
    preview = true;
    updated = true;
    changes = 0;
    idTimer = 0;
    diff = 0.0;

    readOptions();
    setMouseTracking(true);
}

Widget::~Widget()
{
    thread->quit();
    thread->wait();
    delete thread;
    delete image;
}

void Widget::setPreview(bool activated)
{
    preview = activated;
}

void Widget::readOptions()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    int fps = settings.value("fps", DEFAULT_FPS).toInt();

    if (diff == 0.0)
        diff = settings.value("differential", DEFAULT_DIFF).toDouble();

    zoomscale = settings.value("zoomscale", DEFAULT_ZOOM).toDouble();
    moveScale = settings.value("movescale", DEFAULT_MOVE).toInt();
    Thread::setParallelize(settings.value("parallelize", DEFAULT_PARALLELIZE).toBool());

    if (idTimer)
        killTimer(idTimer);

    idTimer = startTimer(1000 / fps);
}

void Widget::resizeEvent(QResizeEvent *)
{
    double sizeX = width() * diff;
    double sizeY = height() * diff;

    if (surface.isNull()) {
        surface.moveLeft(-sizeX / 2.0);
        surface.moveTop(-sizeY / 2.0);

    } else {
        surface.moveLeft(surface.left() + (surface.width() - sizeX) / 2.0);
        surface.moveTop(surface.top() + (surface.height() - sizeY) / 2.0);
    }

    surface.setWidth(sizeX);
    surface.setHeight(sizeY);
    askPaint();
}

bool Widget::event(QEvent *event)
{
    ResultEvent *result;

    if (event->type() == QEvent::User) {
        result = (ResultEvent*)event;

        if (result->code == askCode) {
            delete image;
            image = ((ResultEvent*)event)->image;
            updated = true;
            changes = 0;
            update();
        }

        return true;
    } else
        return QWidget::event(event);
}

void Widget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QString message = QString("X=[") + QString::number(surface.left()) + ":" + QString::number(surface.right()) + "] Y=[" + QString::number(surface.top()) + "i:" + QString::number(surface.bottom()) + tr("i] Resolution: ") + QString::number(diff);
    painter.drawImage(0, 0, *image);
    emit statusMessage(message);
}

void Widget::timerEvent(QTimerEvent *)
{
    if (zoomKey != 0)
        scale(zoomKey == 1 ? 1 / zoomscale : zoomscale);

    if (rightKey || downKey)
        move(QPoint(rightKey * moveScale, downKey * moveScale));
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        mousePrev = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
        move(event->pos() - mousePrev);
        mousePrev = event->pos();
    }

    mouseCur = event->pos();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    scale(event->angleDelta().y() > 0 ? 1 / zoomscale : zoomscale);
}

void Widget::move(const QPoint &moving)
{
    surface.moveLeft(surface.left() - moving.x() * diff);
    surface.moveTop(surface.top() - moving.y() * diff);

    if (preview && updated) {
        *image = image->copy(-moving.x(), -moving.y(), width(), height());

        if (moving.x() > 0) // Draw at left
            Thread::mandelbrot(image, QRect(0, 0, moving.x(), height()), surface.topLeft(), diff);
        else if (moving.x() < 0) // Draw at right
            Thread::mandelbrot(image, QRect(width() + moving.x(), 0, -moving.x(), height()), surface.topLeft(), diff);

        int left = moving.x() > 0 ? moving.x() : 0;
        int right = moving.x() < 0 ? width() + moving.x() : width();

        if (moving.y() > 0) // Draw at top
            Thread::mandelbrot(image, QRect(left, 0, right - left, moving.y()), surface.topLeft(), diff);
        else if (moving.y() < 0) // Draw at bottom
            Thread::mandelbrot(image, QRect(left, height() + moving.y(), right - left, -moving.y()), surface.topLeft(), diff);

        update();
    } else
        askPaint();
}

void Widget::scale(double zoom)
{
    // Zoom < 1.0 -> Increase
    // Zoom > 1.0 -> Decrease

    QPointF mouse(mouseCur - QPointF(width(), height()) / 2.0);

    surface.moveTopLeft(surface.topLeft() + mouse * diff);
    surface.moveLeft(surface.left() + (surface.width() - surface.width() * zoom) / 2.0);
    surface.moveTop(surface.top() + (surface.height() - surface.height() * zoom) / 2.0);
    surface.setWidth(surface.width() * zoom);
    surface.setHeight(surface.height() * zoom);

    diff = surface.width() / width();
    surface.moveTopLeft(surface.topLeft() - mouse * diff);
    askPaint();

    if (preview) {
        QImage im = image->scaled(size() * (1.0 / zoom));
        *image = im.copy(mouseCur.x() / zoom - mouseCur.x(), mouseCur.y() / zoom - mouseCur.y(), width(), height());

        /*if (zoom > 1.0) {
            int left = mouseCur.x() - mouseCur.x() / zoom;
            int right = left + width() / zoom;
            int top = mouseCur.y() -  mouseCur.y() / zoom;
            int bottom = top + height() / zoom;

            Thread::mandelbrot(image, QRect(0, 0, left, height()), surface.topLeft(), diff); // Left
            Thread::mandelbrot(image, QRect(right, 0, width() - right, height()), surface.topLeft(), diff); // Right
            Thread::mandelbrot(image, QRect(left, 0, right - left, top), surface.topLeft(), diff); // Up
            Thread::mandelbrot(image, QRect(left, bottom, right - left, height() - bottom), surface.topLeft(), diff); // Down
        }*/

        update();
    }
}

void Widget::askPaint()
{

    QCoreApplication::removePostedEvents(thread, QEvent::User);
    QCoreApplication::postEvent(thread, new AskEvent(++askCode, rect(), surface.topLeft(), diff));
    updated = false;
}

void Widget::setPalette(int num)
{
    thread->setPalette(num);
    askPaint();
}
