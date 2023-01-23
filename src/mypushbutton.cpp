#include "mypushbutton.h"
#include "utils.h"
#include <QDebug>

MyPushButton::MyPushButton(QWidget *parent) :
    QPushButton(parent)
{
    isFirstIcon = true;
}

void MyPushButton::changeIcon()
{
    if (isFirstIcon)
    {
        isFirstIcon = false;
        this->setIcon(icon2);
    }
    else
    {
        isFirstIcon = true;
        this->setIcon(icon1);
    }
}

void MyPushButton::setIcons(const QString& path1, const QString& path2)
{
    bool ret1 = icon1.load(path1);
    bool ret2 = icon2.load(path2);
    if ((!ret1) || (!ret2))
    {
        qDebug() << "MyPushButton: load icons failed!";
        return;
    }
    this->setFixedSize(Utils::iconSize, Utils::iconSize);
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(icon1);
    this->setIconSize(QSize(Utils::iconSize, Utils::iconSize));
    isFirstIcon = true;
}
