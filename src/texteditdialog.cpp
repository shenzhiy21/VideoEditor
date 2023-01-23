#include "texteditdialog.h"
#include "ui_texteditdialog.h"

TextEditDialog::TextEditDialog(const QString& originalText, const bool mouseTrack, QWidget *parent) :
    QDialog(parent),
    mouseTrack(mouseTrack),
    ui(new Ui::TextEditDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(originalText);
    this->setFixedSize(200, 60);
    this->setWindowTitle("输入文字");
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(sendMessage()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

TextEditDialog::~TextEditDialog()
{
    delete ui;
}

void TextEditDialog::sendMessage()
{
    if (mouseTrack)
    {
        emit textSignal(ui->lineEdit->text(), x, y);
    }
    else
    {
        emit renameSignal(ui->lineEdit->text());
    }
    this->close();
}
