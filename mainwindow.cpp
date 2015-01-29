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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    translator = new QTranslator(this);
    options = new Options(this);
    showingMenu = ui->menubar->isVisible();

    connect(ui->actionSave, SIGNAL(triggered()), SLOT(saveImage()));
    connect(ui->actionPalette_1, SIGNAL(triggered()), SLOT(changePalette()));
    connect(ui->actionPalette_2, SIGNAL(triggered()), SLOT(changePalette()));
    connect(ui->actionPalette_3, SIGNAL(triggered()), SLOT(changePalette()));
    connect(ui->actionPreview, SIGNAL(triggered(bool)), ui->centralwidget, SLOT(setPreview(bool)));
    connect(ui->actionEnglish, SIGNAL(triggered()), SLOT(changeLanguage()));
    connect(ui->actionSpanish, SIGNAL(triggered()), SLOT(changeLanguage()));
    connect(ui->actionItalian, SIGNAL(triggered()), SLOT(changeLanguage()));
    connect(ui->actionOptions, SIGNAL(triggered()), options, SLOT(exec()));
    connect(ui->actionHelp, SIGNAL(triggered()), SLOT(help()));
    connect(ui->centralwidget, SIGNAL(statusMessage(QString)), ui->statusbar, SLOT(showMessage(QString)), Qt::QueuedConnection);
    connect(options, SIGNAL(accepted()), ui->centralwidget, SLOT(readOptions()));

    if (QLocale::system().language() == QLocale::Spanish) {
        translator->load(":/languages/mandelbrot_es");
        ui->actionSpanish->setChecked(true);
    } else if (QLocale::system().language() == QLocale::Italian) {
        translator->load(":/languages/mandelbrot_it");
        ui->actionItalian->setChecked(true);
    } else {
        translator->load(":/languages/mandelbrot_en");
        ui->actionEnglish->setChecked(true);
    }

    qApp->installTranslator(translator);
    ui->retranslateUi(this);
    options->retranslate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int &zoomKey = ui->centralwidget->getZoomKey();
    int &rightKey = ui->centralwidget->getRightKey();
    int &downKey = ui->centralwidget->getDownKey();

    switch (event->key()) {
    case Qt::Key_A:
        if (zoomKey <= 0)
            zoomKey++;
        break;
    case Qt::Key_Z:
        if (zoomKey >= 0)
            zoomKey--;
        break;
    case Qt::Key_Up:
        if (downKey >= 0)
            downKey--;
        break;
    case Qt::Key_Down:
        if (downKey <= 0)
            downKey++;
        break;
    case Qt::Key_Left:
        if (rightKey >= 0)
            rightKey--;
        break;
    case Qt::Key_Right:
        if (rightKey <= 0)
            rightKey++;
        break;
    case Qt::Key_Escape:
        if (isFullScreen()) {
            showNormal();
            ui->statusbar->show();

            if (showingMenu)
                ui->menubar->show();
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int &zoomKey = ui->centralwidget->getZoomKey();
    int &rightKey = ui->centralwidget->getRightKey();
    int &downKey = ui->centralwidget->getDownKey();

    switch (event->key()) {
    case Qt::Key_A:
        zoomKey--;
        break;
    case Qt::Key_Z:
        zoomKey++;
        break;
    case Qt::Key_Up:
        downKey++;
        break;
    case Qt::Key_Down:
        downKey--;
        break;
    case Qt::Key_Left:
        rightKey++;
        break;
    case Qt::Key_Right:
        rightKey--;
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

void MainWindow::saveImage()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QString(), tr("Images (*.png *.jpg)"));

    if (!path.isEmpty()) {
        if (path.endsWith(".png", Qt::CaseInsensitive))
            ui->centralwidget->getImage().save(path, "PNG");
        else if (path.endsWith(".jpg", Qt::CaseInsensitive) || path.endsWith(".jpeg", Qt::CaseInsensitive))
            ui->centralwidget->getImage().save(path, "JPEG");
        else
            ui->centralwidget->getImage().save(path + ".png", "PNG");
    }
}

void MainWindow::changePalette()
{
    QObject *sender = this->sender();

    if (sender == ui->actionPalette_1) {
        ui->centralwidget->setPalette(1);
        ui->actionPalette_1->setChecked(true);
        ui->actionPalette_2->setChecked(false);
        ui->actionPalette_3->setChecked(false);
    } else if (sender == ui->actionPalette_2) {
        ui->centralwidget->setPalette(2);
        ui->actionPalette_1->setChecked(false);
        ui->actionPalette_2->setChecked(true);
        ui->actionPalette_3->setChecked(false);
    } else if (sender == ui->actionPalette_3) {
        ui->centralwidget->setPalette(3);
        ui->actionPalette_1->setChecked(false);
        ui->actionPalette_2->setChecked(false);
        ui->actionPalette_3->setChecked(true);
    }

    setLocale(QLocale(QLocale::Italian));
    ui->retranslateUi(this);
    update();
}

void MainWindow::changeLanguage()
{
    QObject *sender = this->sender();

    if (sender == ui->actionEnglish) {
        translator->load(":/languages/mandelbrot_en");
        ui->actionEnglish->setChecked(true);
        ui->actionSpanish->setChecked(false);
        ui->actionItalian->setChecked(false);
    } else if (sender == ui->actionSpanish) {
        translator->load(":/languages/mandelbrot_es");
        ui->actionEnglish->setChecked(false);
        ui->actionSpanish->setChecked(true);
        ui->actionItalian->setChecked(false);
    } else if (sender == ui->actionItalian) {
        translator->load(":/languages/mandelbrot_it");
        ui->actionEnglish->setChecked(false);
        ui->actionSpanish->setChecked(false);
        ui->actionItalian->setChecked(true);
    }

    ui->retranslateUi(this);
    options->retranslate();
}

void MainWindow::help()
{
    QMessageBox::information(this, tr("Help"), tr("A - Zoom+\nZ - Zoom-"));
}
