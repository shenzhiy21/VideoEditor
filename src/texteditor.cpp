#include "texteditor.h"
#include "ui_texteditor.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QColor>
#include <QPalette>
#include <QFont>

TextEditor::TextEditor(TextTimeLine& text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextEditor),
    text(text)
{
    ui->setupUi(this);

    connect(ui->colorBtn, SIGNAL(clicked()), this, SLOT(createColorDialog()));
    connect(ui->fontBtn, SIGNAL(clicked()), this, SLOT(createFontDialog()));
    connect(ui->configBtn, SIGNAL(clicked()), this, SLOT(configText()));

    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText(text.text);
    ui->label->show();
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::createColorDialog()
{
    QColorDialog* dialog = new QColorDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
}

void TextEditor::createFontDialog()
{
    QFontDialog* dialog = new QFontDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(fontSelected(QFont)), this, SLOT(setFont(QFont)));
}

void TextEditor::setColor(QColor color)
{
    text.color = color;
    QPalette pe;
    pe.setColor(QPalette::WindowText, color);
    ui->label->setPalette(pe);
}

void TextEditor::setFont(QFont font)
{
    text.font = font;
    ui->label->setFont(font);
}

void TextEditor::configText()
{
    emit addText(text);
    this->close();
}
