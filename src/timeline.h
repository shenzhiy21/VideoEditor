#ifndef TIMELINE_H
#define TIMELINE_H

// 时间轴类
#include "videotimeline.h"
#include "imagetimeline.h"
#include "subtitleprocesser.h"
#include "texttimeline.h"
#include <QPaintEvent>
#include <QWidget>
#include <QLabel>

class TimeLine : public QWidget
{
    Q_OBJECT
public slots:
    void updateVideo();
    void setFrame(int);
    void addImage(const QRect&, const QString&);      // 添加图片素材
    void receiveText(TextTimeLine);     // 添加文字素材
    void replaceText(TextTimeLine);     // 替换文本素材
    void tryFade(int, int);             // 添加淡入淡出特效
    void tryMove(int);             // 添加平移特效
    void tryScale(double);          // 添加缩放特效

public:
    explicit TimeLine(QWidget *parent = nullptr);
    void addNewVideo(const QString& path);
    void addSubtitle(const SubtitleProcesser& subtitleProcesser);
    void cutVideoByFrame();     // 剪切视频
    bool hasVideos() const;   // 是否有视频在时间轴里
    void merge();       // 合并两个视频
    void deleteSingleResource();   // 删除当前素材
    void swap(int offset);      // 将当前视频与和它相距 offset 的视频交换顺序
    int setToBegin();          // 将当前视频设置到第一帧，并返回当前帧
    int setToEnd();            // 将当前视频设置到最后一帧，并返回当前帧
    void offset(int val);        // 给当前帧一个 val 大小的偏移
    int totalFrames() const;    // 总共有多少帧
    int currentFrame() const;   // 当前是第几帧
    void addImageByDefault(const QString& path);    // 将图片默认添加到画面的 (0, 0) 处
    void output();  // 导出视频
    void changeResourseState(int);     // 改变素材的时间位置及时长

    QList<VideoTimeLine> videosInTimeLine;   // 时间轴中的视频素材
    QList<ImageTimeLine> imagesInTimeLine;   // 时间轴中的图片素材
    QList<TextTimeLine> textsInTimeLine;     // 时间轴中的文字素材

    int fps;    // 帧率

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool isValid() const;
    void mergeTwoVideos(int first, int second);
    int getFrames(const int rank, const int frame) const; // 根据某个 rank 和 frame 值，求出这一时刻的总帧数
    QList<ImageTimeLine> getImages(const int curFrame) const; // 获得当前帧应当显示的所有图片信息
    QList<TextTimeLine> getTexts(const int curFrame) const;     // 获得当前帧应当显示的所有文字信息
    void changeTrack(int yPos);     // 改变当前选中的轨道

    int videoRank;  // 当前是第几个视频
    int videoFrame; // 当前视频的第几帧
    int currentTrack;  // 当前选中的轨道（视频 or 图片 or 文字）

signals:
    void imageChanged(int, const QImage&, const QList<ImageTimeLine>&, const QList<TextTimeLine>&);   // 视频切换到下一帧了
    void timeLineChanged(int);        // 时间轴发生改变了（int 代表视频总帧数）
    void currentFrameChanged(int);      // 当前帧变化（int 代表当前是第几帧）
};

#endif // TIMELINE_H
