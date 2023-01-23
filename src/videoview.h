#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QPixmap>

// 展示在视频列表里的视频素材
class VideoView
{
public:
    VideoView(const QString& dir, const QString& path, const int fps);
    VideoView() {}
    void deleteBuffer();

    QString dir;
    QString path;
    QString buffer;
    QString name;
    QPixmap pixmap;
    bool isValid = false;

private:
    int fps;
    int duration;

    void ffmpegSplit();
    void loadImages();
};

#endif // VIDEOVIEW_H
