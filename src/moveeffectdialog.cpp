#include "moveeffectdialog.h"
#include "ui_moveeffectdialog.h"

MoveEffectDialog::MoveEffectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveEffectDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("平移特效");

    ui->spinBox->setMaximum(1000);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addMoveEffect()));
}

MoveEffectDialog::~MoveEffectDialog()
{
    delete ui;
}

void MoveEffectDialog::addMoveEffect()
{
    emit sendMoveMessage(ui->spinBox->value());
    this->close();
}
