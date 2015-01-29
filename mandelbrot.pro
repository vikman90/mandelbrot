#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T16:24:21
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32: QMAKE_CXXFLAGS += -openmp

unix {
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
}

TARGET = mandelbrot
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mainwindow.cpp \
    thread.cpp \
    options.cpp

HEADERS  += widget.h \
    mainwindow.h \
    thread.h \
    resultevent.h \
    askevent.h \
    options.h

FORMS += \
    mainwindow.ui \
    options.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += languages/mandelbrot_es.ts \
    languages/mandelbrot_it.ts \
    languages/mandelbrot_en.ts
