#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

// 文本编辑的对话框
#include <QDialog>

namespace Ui {
class TextEditDialog;
}

class TextEditDialog : public QDialog
{
    Q_OBJECT

public slots:
    void sendMessage();

public:
    explicit TextEditDialog(const QString& originalName, const bool mouseTrack = false, QWidget *parent = nullptr);
    ~TextEditDialog();

    int x;
    int y;
    bool mouseTrack;

signals:
    void textSignal(QString, int, int);
    void renameSignal(QString);

private:
    Ui::TextEditDialog *ui;
};

#endif // TEXTEDITDIALOG_H
