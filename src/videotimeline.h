#ifndef VIDEOTIMELINE_H
#define VIDEOTIMELINE_H

// 时间轴中的视频素材
#include <QString>
#include <QImage>

class VideoTimeLine
{
public:
    VideoTimeLine(const QString& path, const int fps);
    VideoTimeLine(const VideoTimeLine& other, const int begin, const int end);
    void mergeWith(const VideoTimeLine& other);

    int fps;    // 帧率
    QList<QImage> images;   // 存储的每一帧图片
    double duration;    // 视频的总时长（单位：秒）

private:
    QString imgPath;    // 存图片的地址
};

#endif // VIDEOTIMELINE_H
