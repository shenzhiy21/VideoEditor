#include "subtitleprocesser.h"
#include "utils.h"
#include <fstream>
#include <QStringList>
#include <cassert>
#include <QDebug>
using namespace std;
SubtitleProcesser::SubtitleProcesser(const string& path) :
    path(path)
{
    if (path == "") isValid = false;
    else isValid = true;
    name = Utils::getRelativeName(QString::fromStdString(path));
}

bool SubtitleProcesser::process()
{
    ifstream fin;
    fin.open(path, ios::in);

    string str;
    int line = 0;   // 当前在第几行（文件首行对应 line = 1）

    // .SRT 字幕的格式如下：

    // -------------
    // 1
    // 00:00:01,836 --> 00:00:04,504
    // Police Radio Chatter
    //
    // -------------

    // 第一行代表标号
    // 第二行代表时间
    // 第三行代表文本
    // 第四行为一个空行

    while (getline(fin, str))
    {
        line++;
        int mode = line % 4;
        // 第一行：创建一个字幕结构体
        if (mode == 1)
        {
            Subtitle subtitle;
            subtitles.push_back(subtitle);
        }
        // 第二行：为其新增时间节点
        else if (mode == 2)
        {
            QString timeText = QString::fromStdString(str);

            // 先根据空格字符切片
            QStringList timeList = timeText.split(" ");
            if (timeList.size() != 3)
            {
                qDebug() << "Subtitle Processer: time split fail!";
                return false;
            }
            int st = timeProcess(timeList[0]);
            int et = timeProcess(timeList[2]);
            if ((st < 0) || (et < 0)) return false;
            subtitles.at(subtitles.size() - 1).startTime = timeProcess(timeList[0]);
            subtitles.at(subtitles.size() - 1).endTime = timeProcess(timeList[2]);
        }
        // 第三行：为其新增字幕文本
        else if (mode == 3) subtitles.at(subtitles.size() - 1).text = str;
        else continue;
    }
    return true;
}

int SubtitleProcesser::timeProcess(const QString& time)
{
    QStringList time1 = time.split(":");
    if (time1.size() != 3) return -1;
    int hour = time1[0].toInt();
    int min = time1[1].toInt();
    QStringList time2 = time1[2].split(",");
    if (time2.size() != 2) return -1;
    int sec = time2[0].toInt();
    int millisec = time2[1].toInt();
    return millisec + (hour * 3600 + min * 60 + sec) * 1000;
}
