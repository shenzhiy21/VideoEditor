#include "videoview.h"
#include "ffmpeg.h"
#include "utils.h"
#include <QProcess>
#include <QDir>
#include <QDebug>

VideoView::VideoView(const QString& dir, const QString& path, const int fps)
    : dir(dir), path(path), fps(fps)
{
    name = Utils::getRelativeName(path);
    ffmpegSplit();
    loadImages();
    isValid = true;
}

// 将视频抽帧，并将得到的图片保存下来
void VideoView::ffmpegSplit()
{
    // 创建缓存文件夹
    buffer = dir + "\\Buffer\\" + name;
    QDir qDir;
    qDir.mkdir(buffer);

    // 抽帧
    QString outputPath = buffer + "\\%d.jpg";
    ffmpeg::split2Imgs(path, outputPath, fps);
}

// 创建视频列表的图标，并把图片存入 VideoView 类中
void VideoView::loadImages()
{
    QString imagePath = buffer + "\\1.jpg";
    bool flag = pixmap.load(imagePath);
    if (!flag)
    {
        qDebug() << "image load fail!";
    }
}

void VideoView::deleteBuffer()
{
    QDir qDir;
    qDir.setPath(buffer);
    qDir.removeRecursively();
}
