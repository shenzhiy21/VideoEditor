#ifndef MYSLIDER_H
#define MYSLIDER_H

// 继承 QSlider 的滑块控件
#include <QWidget>
#include <QSlider>

class MySlider : public QSlider
{
    Q_OBJECT
public slots:
    void setTotalFrame(int);        // 设置总帧数
    void setCurrentFrame(int);      // 设置当前帧数

public:
    explicit MySlider(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

signals:
    void mousePress(int value);
    void mouseRelease(int value);

};

#endif // MYSLIDER_H
