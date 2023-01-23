#ifndef FADEEFFECTDIALOG_H
#define FADEEFFECTDIALOG_H

// 淡入淡出特效的对话框
#include <QDialog>

namespace Ui {
class FadeEffectDialog;
}

class FadeEffectDialog : public QDialog
{
    Q_OBJECT

private slots:
    void addFadeEffect();

public:
    explicit FadeEffectDialog(QWidget *parent = nullptr);
    ~FadeEffectDialog();

private:
    Ui::FadeEffectDialog *ui;

signals:
    void sendFadeMessage(int, int);
};

#endif // FADEEFFECTDIALOG_H
