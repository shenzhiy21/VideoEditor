#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

// 维护了素材库中的图片信息
#include <QPixmap>

class ImageView
{
public:
    ImageView(const QString& path);

    ImageView() : isValid(false) {}

    QString path;       // 路径

    QString name;       // 名字

    QPixmap pixmap;     // 图标

    bool isValid;       // 是否合法
};

#endif // IMAGEVIEW_H
