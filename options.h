#ifndef OPTIONS_H
#define OPTIONS_H

#define ORGANIZATION "vikman"
#define APPLICATION "mandelbrot"
#define DEFAULT_DIFF 0.005
#define DEFAULT_ZOOM 1.1
#define DEFAULT_MOVE 5
#define DEFAULT_FPS 20
#define DEFAULT_PARALLELIZE true

#include <QDialog>
#include <QSettings>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

    void retranslate();

public slots:
    void defaults();
    virtual void accept();

private:
    Ui::Options *ui;
    QSettings *settings;
};

#endif // OPTIONS_H
