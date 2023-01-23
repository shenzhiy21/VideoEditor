#include "imagetimeline.h"
#include <QPainter>
#include <QDebug>

ImageTimeLine::ImageTimeLine(const QRect& rect, const QString& path)
    : rect(rect), path(path)
{
    bool ret = image.load(path);
    if (!ret)
    {
        qDebug() << "ImageTimeLine: image load fail!";
    }
}
