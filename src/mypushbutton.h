#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

// 自定义的 PushButton 类，维护了一个可以有两种图标的 pushButton
#include <QPushButton>
#include <QPixmap>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public slots:
    void changeIcon();  // 切换图标

public:
    explicit MyPushButton(QWidget *parent = nullptr);
    void setIcons(const QString& path1, const QString& path2);
    bool isFirstIcon;   // 是否为第一个图标

private:
    QPixmap icon1;
    QPixmap icon2;

signals:

};

#endif // MYPUSHBUTTON_H
