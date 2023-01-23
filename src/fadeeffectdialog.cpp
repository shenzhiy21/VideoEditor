#include "fadeeffectdialog.h"
#include "ui_fadeeffectdialog.h"

FadeEffectDialog::FadeEffectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FadeEffectDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("淡入淡出");
    ui->spinBox->setMaximum(10000);
    ui->spinBox_2->setMaximum(10000);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addFadeEffect()));
}

FadeEffectDialog::~FadeEffectDialog()
{
    delete ui;
}

void FadeEffectDialog::addFadeEffect()
{
    emit sendFadeMessage(ui->spinBox->value(), ui->spinBox_2->value());
    this->close();
}
