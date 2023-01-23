#include "ffmpeg.h"
#include <QProcess>
#include <QDebug>

void ffmpeg::split2Imgs(const QString& inputPath, const QString& outputPath, const int fps)
{
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    QStringList ffmpegProcess;
    ffmpegProcess << "-i" << inputPath << "-r" << QString::number(fps) << outputPath;
    process.start("ffmpeg", ffmpegProcess);
    process.waitForStarted();
    process.waitForFinished();
    qDebug() << "load video finished!";
}

void ffmpeg::cutVideo(const QString& inputPath, const QString& outputPath, const QString& startTime, const QString& duration)
{
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    QStringList ffmpegProcess;
    ffmpegProcess << "-i" << inputPath << "-ss" << startTime << "-t" << duration << outputPath;
    process.start("ffmpeg", ffmpegProcess);
    process.waitForStarted();
    process.waitForFinished();
    qDebug() << "cut video finished";
}

void ffmpeg::output(const QString& inputPath, const QString& outputPath, const int fps)
{
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    QStringList ffmpegProcess;
    QString inputImagePath = inputPath + "\\%d.jpg";
    ffmpegProcess << "-r" << QString::number(fps) << "-i" << inputImagePath << outputPath;
    process.start("ffmpeg", ffmpegProcess);
    process.waitForStarted();
    process.waitForFinished(-1);
    qDebug() << "video output finished";
}
