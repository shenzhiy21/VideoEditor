#ifndef MOVEEFFECTDIALOG_H
#define MOVEEFFECTDIALOG_H

#include <QDialog>

namespace Ui {
class MoveEffectDialog;
}

class MoveEffectDialog : public QDialog
{
    Q_OBJECT

private slots:
    void addMoveEffect();

public:
    explicit MoveEffectDialog(QWidget *parent = nullptr);
    ~MoveEffectDialog();

private:
    Ui::MoveEffectDialog *ui;

signals:
    void sendMoveMessage(int);
};

#endif // MOVEEFFECTDIALOG_H
