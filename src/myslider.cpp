#include "myslider.h"
#include <QMouseEvent>

MySlider::MySlider(QWidget *parent)
    : QSlider{parent}
{

}

void MySlider::mousePressEvent(QMouseEvent* event)
{
    int currentX = event->pos().x();
    double percentage = currentX * 1.0 / this->width();
    int value = percentage * (this->maximum() - this->minimum()) + this->minimum();
    this->setValue(value);

    emit mousePress(value);
    QSlider::mousePressEvent(event);
    qDebug() << "mouse press:" << value;
}

void MySlider::mouseReleaseEvent(QMouseEvent* event)
{
    int currentX = event->pos().x();
    double percentage = currentX * 1.0 / this->width();
    int value = percentage * (this->maximum() - this->minimum()) + this->minimum();
    this->setValue(value);

    emit mouseRelease(value);
    QSlider::mouseReleaseEvent(event);
    qDebug() << "mouse release:" << value;
}

void MySlider::setTotalFrame(int duration)
{
    this->setMaximum(duration);
    this->setMinimum(0);
}

void MySlider::setCurrentFrame(int duration)
{
    if (duration < this->minimum()) duration = this->minimum();
    else if (duration > this->maximum()) duration = this->maximum();
    this->setValue(duration);
}
