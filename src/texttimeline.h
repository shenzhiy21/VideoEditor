#ifndef TEXTTIMELINE_H
#define TEXTTIMELINE_H

// 时间轴中的文本信息
#include <QString>
#include <QFont>
#include <QColor>
#include <QRect>

class TextTimeLine
{
public:
    TextTimeLine();
    TextTimeLine(const QString& text, const int startFrame, const int endFrame);

    QString text;
    QFont font;
    QColor color;
    QRect rect;
    int startFrame;
    int endFrame;
};

#endif // TEXTTIMELINE_H
