#ifndef FFMPEG_H
#define FFMPEG_H

// ffmpeg + QProcess 的指令集
#include <QString>
namespace ffmpeg
{
    void split2Imgs(const QString& inputPath, const QString& outputPath, const int fps);  // 抽帧
    void cutVideo(const QString& inputPath, const QString& outputPath, const QString& startTime, const QString& duration);  // 裁剪
    void output(const QString& inputPath, const QString& outputPath, const int fps);      // 导出
};

#endif // FFMPEG_H
