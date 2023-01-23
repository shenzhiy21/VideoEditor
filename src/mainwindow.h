#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imageview.h"
#include "videoview.h"
#include "subtitleprocesser.h"
#include "timeline.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QListWidget>
#include <QTimer>
#include <QTemporaryDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectFiles();     // 选择操作
    void removeFiles();     // 删除操作
    void renameFiles();     // 重命名操作
    void showVideoCut();    // 显示出裁剪视频的对话框
    void changeName(QString);   // 重命名
    void sliderPressEffect(int);   // 滑块按下
    void sliderReleaseEffect(int);  // 滑块松开
    void sliderMoveEffect(int);     // 滑块移动
    void cutVideo();        // 剪视频
    void addNewVideo(const QString&);  // 根据路径，添加新文件
    void addResource();   // 将素材添加到时间轴中
    void startPlayVideo();  // 预览窗口开始播放视频
    void stopPlayVideo();   // 预览窗口停止播放视频
    void playpauseAction(); // 控制预览窗口播放与否的槽
    void mergeVideos();     // 合并视频
    void deleteResource();     // 删除视频
    void forwardSwap();     // 将当前视频与它前一个视频交换位置
    void backwardSwap();    // 将当前视频与它后一个视频交换位置
    void toBegin();     // 将预览窗口的时间调回第一帧
    void toEnd();       // 将预览窗口的时间调到最后一帧
    void oneFrameBefore();  // 将视频往前调一帧
    void oneFrameAfter();   // 将视频往后调一帧
    void createQDrag(QListWidgetItem*); // 创建一个 QDrag 以供拖拽到预览窗口
    void outputVideo();     // 导出视频
    void showFadeDialog();      // 淡入淡出特效对话框
    void showMoveDialog();      // 平移特效对话框
    void showSizeDialog();      // 缩放特效对话框

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    Ui::MainWindow *ui;
    QFileDialog* fileSelector;      // 选择文件的对话框
    QList<VideoView> videoList;     // 视频列表
    QList<ImageView> imageList;     // 图片列表
    QList<SubtitleProcesser> subtitleList;  // 字幕列表
    QString bufferDir;  // 缓存文件的文件夹
    QString tempDir;    // 导出时缓存信息的文件夹
    TimeLine* timeLine; // 时间轴
    QTimer* timer;  // 模拟视频播放的计时器
    QTemporaryDir* dir; // 临时文件夹，用于存放中间数据
    bool isPlay;    // 是否在预览

    int fileType() const;   // 获取当前的素材类型（视频 or 图片 or 字幕）
    const VideoView selectVideo() const;    // 返回选中的视频素材
    const ImageView selectImage() const;    // 返回选中的图片素材
    const SubtitleProcesser selectSubtitles() const;    // 返回选中的字幕素材

};
#endif // MAINWINDOW_H
