#include "imageview.h"
#include "utils.h"

ImageView::ImageView(const QString& path)
    : path(path)
{
    name = Utils::getRelativeName(path);
    isValid = pixmap.load(path);    // 根据是否成功导入图片，来判断该类是否合法
}
