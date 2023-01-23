#ifndef UTILS_H
#define UTILS_H

#include <QPushButton>
#include <QString>

namespace Utils
{
    QString getRelativeName(const QString& url);
    QString calculateTime(const qint64 time); //计算出形如 "xx:xx.xxx" 的时间
    QString calculateSecondTime(const qint64 time); //计算出形如 "xx.xxx" 的时间
    QString calculateThirdTime(const int seconds);  // 计算出形如 "xx:xx" 的时间
    void mySetIcon(QPushButton* btn, const QString& path);  // 将 btn 的 icon 设置为 path 对应图标，并进行一系列操作
    enum FileType {
        VIDEO,
        IMAGE,
        SUBTITLE,
        NOTYPE
    };
    const int fps = 20;
    const int lengthPerSec = 50;    // 时间轴上每秒钟对应宽度
    const int secForSingleRect = 2; // 时间轴上单个矩形对应的时长
    const double singleRectWidth = lengthPerSec * secForSingleRect;    // 时间轴上单个视频帧矩形的宽度
    const double singleRectHeight = singleRectWidth * 0.6;  // 时间轴上单个视频帧矩形的高度
    const int iconSize = 31;    // 图标的边长
}

#endif // UTILS_H
