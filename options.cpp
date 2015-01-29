#include "options.h"
#include "ui_options.h"
#include <QtCore>

Options::Options(QWidget *parent) : QDialog(parent), ui(new Ui::Options)
{
    settings = new QSettings(ORGANIZATION, APPLICATION, this);
    ui->setupUi(this);
    connect(ui->btDefaults, SIGNAL(clicked()), SLOT(defaults()));
    ui->spDiff->setValue(settings->value("differential", DEFAULT_DIFF).toDouble());
    ui->spZoom->setValue(settings->value("zoomscale", DEFAULT_ZOOM).toDouble());
    ui->spMove->setValue(settings->value("movescale", DEFAULT_MOVE).toInt());
    ui->spFps->setValue(settings->value("fps", DEFAULT_FPS).toInt());
    ui->cbParallelize->setChecked(settings->value("parallelize", DEFAULT_PARALLELIZE).toBool());
}

Options::~Options()
{
    delete ui;
}

void Options::retranslate()
{
    ui->retranslateUi(this);
}

void Options::defaults()
{
    ui->spDiff->setValue(DEFAULT_DIFF);
    ui->spZoom->setValue(DEFAULT_ZOOM);
    ui->spMove->setValue(DEFAULT_MOVE);
    ui->spFps->setValue(DEFAULT_FPS);
    ui->cbParallelize->setChecked(DEFAULT_PARALLELIZE);
}

void Options::accept()
{
    settings->setValue("differential", ui->spDiff->value());
    settings->setValue("zoomscale", ui->spZoom->value());
    settings->setValue("movescale", ui->spMove->value());
    settings->setValue("fps", ui->spFps->value());
    settings->setValue("parallelize", ui->cbParallelize->isChecked());
    QDialog::accept();
}
