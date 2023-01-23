#include "utils.h"
#include <QStringList>
#include <QDebug>

QString Utils::getRelativeName(const QString& url)
{
    QStringList list1 = url.split('/');
    QString ret1 = list1.at(list1.size() - 1);
    QStringList list2 = ret1.split('.');
    QString ret2 = list2.at(0);
    return ret2;
}

QString Utils::calculateTime(const qint64 time)
{
    int secTotal = time / 1000;
    int min = secTotal / 60;
    int second = secTotal - min * 60;
    int millisec = time - secTotal * 1000;
    return (QString::number(min) + ":" + QString::number(second) + "." + QString::number(millisec));
}

QString Utils::calculateSecondTime(const qint64 time)
{
    int second = time / 1000;
    int millisec = time - second * 1000;
    return (QString::number(second) + "." + QString::number(millisec));
}

void Utils::mySetIcon(QPushButton* btn, const QString& path)
{
    QPixmap pixmap;
    bool ret = pixmap.load(path);
    if (!ret) qDebug() << "Button: load icon failed!";

    btn->setFixedSize(Utils::iconSize, Utils::iconSize);
    btn->setStyleSheet("QPushButton {border:0px;}");
    btn->setIcon(pixmap);
    btn->setIconSize(QSize(Utils::iconSize, Utils::iconSize));
}

QString Utils::calculateThirdTime(const int seconds)
{
    int min = seconds / 60;
    int sec = seconds - min * 60;
    QString secText = (sec < 10) ? ("0" + QString::number(sec)) : QString::number(sec);
    return ("0" + QString::number(min) + ":" + secText);
}
