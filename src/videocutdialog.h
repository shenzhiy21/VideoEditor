#ifndef VIDEOCUTDIALOG_H
#define VIDEOCUTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QKeyEvent>

namespace Ui {
class VideoCutDialog;
}

class VideoCutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCutDialog(const QString& path, QWidget *parent = nullptr);
    ~VideoCutDialog();

public slots:
    void getDuration(qint64);
    void getPosition(qint64);
    void playerPause(); // 播放器暂停
    void playerPlay();  // 播放器播放
    void getStartTime();    // 设置裁剪的 startTime
    void getEndTime();      // 设置裁剪的 endTime
    void checkOut();        // 确认裁剪

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    Ui::VideoCutDialog *ui;
    QString path;   // 视频路径
    QMediaPlayer* player;   // 视频播放器
    bool sliderState;   // 滑块是否被选中
    QMediaPlayer::PlaybackState playbackState;  // 播放器状态
    qint64 startTime = -1;   // 待裁剪片段开始的时间点
    qint64 endTime = -1;     // 待裁剪片段终止的时间点

    void showVideo();   // 播放视频
    void setDuration(qint64 playtime);  // 设置视频总时长
    void setPosition(qint64 playtime);  // 设置指针当前位置

signals:
    void cutVideoMessage(const QString&);      // 裁剪后的文件信息.参数代表路径
};

#endif // VIDEOCUTDIALOG_H
