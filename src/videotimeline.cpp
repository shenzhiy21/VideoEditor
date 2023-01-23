#include "videotimeline.h"
#include <QDebug>

VideoTimeLine::VideoTimeLine(const QString& path, const int fps) :
    fps(fps), imgPath(path)
{
    // 导入图片
    QImage img;
    int index = 1;
    while (true)
    {
        QString curImgPath = path + "\\" + QString::number(index) + ".jpg";
        index++;
        img.load(curImgPath);
        if (img.isNull()) break;    // 没有可导入的图片了
        else
        {
            images.push_back(img);
        }
    }
    qDebug() << "image size:" << images.size();
    duration = (double)images.size() / fps;
}

VideoTimeLine::VideoTimeLine(const VideoTimeLine& other, const int begin, const int end)
{
    fps = other.fps;
    imgPath = other.imgPath;
    for (int i = begin; i < end; ++i)
    {
        images.push_back(other.images[i]);
    }
    duration = (double)images.size() / fps;
}

void VideoTimeLine::mergeWith(const VideoTimeLine& other)
{
    for (int i = 0; i < other.images.size(); ++i)
    {
        images.push_back(other.images[i]);
    }
    duration = (double)images.size() / fps;
}
