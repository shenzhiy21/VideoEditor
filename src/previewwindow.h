#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

// 预览窗口控件
#include "imagetimeline.h"
#include "texttimeline.h"
#include <QWidget>
#include <QPaintEvent>
#include <QImage>
#include <QLabel>

class PreviewWindow : public QWidget
{
    Q_OBJECT
public slots:
    void showImage(int, const QImage&, const QList<ImageTimeLine>&, const QList<TextTimeLine>&);
    void addTextToTimeLine(TextTimeLine);

private slots:
    void showTextEditor(QString, int, int);   // 显示文本样式编辑器

public:
    explicit PreviewWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QLabel* videoLabel;         // 画视频
    QList<QLabel*> imageLabels; // 画图片
    QList<QLabel*> textLabels;  // 画文本（包括字幕）

    void createTextDrag(QLabel* label);     // 创建文本拖拽

signals:
    void imageToAdd(const QRect&, const QString&);
    void textToLoad(TextTimeLine);  // 将一个文本信息导入到时间轴中
    void textToReplace(TextTimeLine);   // 将一个需要替换的文本信息导入到时间轴中
};

#endif // PREVIEWWINDOW_H
