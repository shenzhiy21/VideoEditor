#ifndef SIZEEFFECTDIALOG_H
#define SIZEEFFECTDIALOG_H

// 缩放特效的对话框
#include <QDialog>

namespace Ui {
class SizeEffectDialog;
}

class SizeEffectDialog : public QDialog
{
    Q_OBJECT

private slots:
    void addSizeEffect();

public:
    explicit SizeEffectDialog(QWidget *parent = nullptr);
    ~SizeEffectDialog();

private:
    Ui::SizeEffectDialog *ui;

signals:
    void sendSizeMessage(double);
};

#endif // SIZEEFFECTDIALOG_H
