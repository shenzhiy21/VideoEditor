#ifndef SUBTITLEPROCESSER_H
#define SUBTITLEPROCESSER_H

// 字幕文件解析器，用于处理 .srt 格式的字幕文件
#include <string>
#include <vector>
#include <QString>

struct Subtitle
{
    int startTime;  // 单位：毫秒
    int endTime;    // 单位：毫秒
    std::string text;
};

class SubtitleProcesser
{
public:
    SubtitleProcesser(const std::string& path = "");
    bool process();

    std::vector<Subtitle> subtitles;
    QString name;
    std::string path;
    bool isValid;

private:
    int timeProcess(const QString& time);
};

#endif // SUBTITLEPROCESSER_H
