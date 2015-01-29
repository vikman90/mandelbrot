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

#include "thread.h"
#include <QtWidgets>
#include "widget.h"
#include "askevent.h"
#include "resultevent.h"

static int rgbFromWaveLength(double wave);
bool Thread::parallelize = true;

Thread::Thread(Widget *widget)
{
    this->widget = widget;
    palette.resize(256);
    setPalette(1);
    moveToThread(this);
    start();
}

bool Thread::event(QEvent *event)
{
    AskEvent *askEvent;
    QImage *image;

    if (event->type() == QEvent::User) {
        askEvent = (AskEvent*)event;
        image = new QImage(askEvent->window.size(), QImage::Format_Indexed8);

        mutexPalette.lock();
        image->setColorTable(palette);
        mutexPalette.unlock();

        mandelbrot(image, askEvent->window, askEvent->offset, askEvent->diff);
        qApp->postEvent(widget, new ResultEvent(askEvent->code, image));
        return true;

    } else {
        return QThread::event(event);
    }
}

void Thread::mandelbrot(QImage *image, const QRect &fragment, const QPointF &offset, double diff)
{
    if (parallelize) {
#pragma omp parallel for schedule(dynamic)
        for (int x = fragment.left(); x <= fragment.right(); x++) {
            for (int y = fragment.top(); y <= fragment.bottom(); y++) {
                double cr = diff * x + offset.x();  // c_real
                double ci = diff * y + offset.y();  // c_imag
                double zr = cr;                     // z_real
                double zi = ci;                     // z_imag
                double zrs = zr * zr;               // z_real^2
                double zis = zi * zi;               // z_imag^2
                uint color;

                // Stop when the distance to center exceeds of 2: sqrt(zr^2 + zi^2) >= 2
                for (color = 0; color < 255 && zrs + zis < 4.0; color++) {
                    // Sucession: z = z^2 + c
                    zi = zr * zi;
                    zi += zi + ci;
                    zr = zrs - zis + cr;
                    zrs = zr * zr;
                    zis = zi * zi;
                }

                image->setPixel(x, y, color);
            }
        }
    } else {
        for (int x = fragment.left(); x <= fragment.right(); x++) {
            for (int y = fragment.top(); y <= fragment.bottom(); y++) {
                double cr = diff * x + offset.x();  // c_real
                double ci = diff * y + offset.y();  // c_imag
                double zr = cr;                     // z_real
                double zi = ci;                     // z_imag
                double zrs = zr * zr;               // z_real^2
                double zis = zi * zi;               // z_imag^2
                uint color;

                // Stop when the distance to center exceeds of 2: sqrt(zr^2 + zi^2) >= 2
                for (color = 0; color < 255 && zrs + zis < 4.0; color++) {
                    // Sucession: z = z^2 + c
                    zi = zr * zi;
                    zi += zi + ci;
                    zr = zrs - zis + cr;
                    zrs = zr * zr;
                    zis = zi * zi;
                }

                image->setPixel(x, y, color);
            }
        }
    }
}

void Thread::setPalette(int num)
{
    mutexPalette.lock();

    switch (num) {
    case 1:
        for (int i = 0; i < 256; i++)
            palette[i] = rgbFromWaveLength(380.0 + (i * 400.0 / 255.0));

        palette[255] = qRgb(48, 0, 0);
        break;

    case 2:
        for (int i = 0; i < 256; i++)
            palette[i] = qRgb((255 - i) * 195 / 255 + 60, (255 - i) * 195 / 255 + 17, (255 - i) * 195 / 255);

        palette[255] = qRgb(30, 8, 0);
        break;


    case 3:
        for (int i = 0; i < 256; i++)
            palette[i] = qRgb(127 - i / 2, 127 - i / 4, 255 - i);

        palette[255] = qRgb(0, 32, 0);
    }

    mutexPalette.unlock();
}

static int rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }

    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

    r = pow(r * s, 0.8);
    g = pow(g * s, 0.8);
    b = pow(b * s, 0.8);
    return qRgb(int(r * 255), int(g * 255), int(b * 255));
}
