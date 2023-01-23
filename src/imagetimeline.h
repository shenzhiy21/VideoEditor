#ifndef IMAGETIMELINE_H
#define IMAGETIMELINE_H

#include <QRect>
#include <QString>
#include <QImage>

// 维护了时间轴中的图片
class ImageTimeLine
{
public:
    explicit ImageTimeLine(const QRect& rect, const QString& path);
    int startFrame;     // 起始帧
    int endFrame;       // 结束帧
    QImage image;       // 图片
    QRect rect;         // 位置

    bool isFade = false;    // 是否淡入淡出
    int fadeInFrames;    // 淡入帧数
    int fadeOutFrames;   // 淡出帧数

    bool isMove = false;    // 是否平移
    int moveDistance;     // 平移距离

    bool isScale = false;   // 是否缩放
    double scaleRatio;      // 缩放比率

private:
    QString path;       // 路径
};

#endif // IMAGETIMELINE_H
