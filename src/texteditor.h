#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

// 文本样式编辑器，可设置 font 和 color
#include "texttimeline.h"
#include <QDialog>

namespace Ui {
class TextEditor;
}

class TextEditor : public QDialog
{
    Q_OBJECT

private slots:
    void createColorDialog();
    void createFontDialog();
    void setColor(QColor);
    void setFont(QFont);
    void configText();

public:
    explicit TextEditor(TextTimeLine& text, QWidget *parent = nullptr);
    ~TextEditor();

private:
    Ui::TextEditor *ui;

    TextTimeLine text;

signals:
    void addText(TextTimeLine);
};

#endif // TEXTEDITOR_H
