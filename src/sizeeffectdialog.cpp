#include "sizeeffectdialog.h"
#include "ui_sizeeffectdialog.h"

SizeEffectDialog::SizeEffectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SizeEffectDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("缩放特效");

    ui->doubleSpinBox->setRange(0.1, 10);
    ui->doubleSpinBox->setValue(1.0);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addSizeEffect()));
}

SizeEffectDialog::~SizeEffectDialog()
{
    delete ui;
}

void SizeEffectDialog::addSizeEffect()
{
    emit sendSizeMessage(ui->doubleSpinBox->value());

    this->close();
}
