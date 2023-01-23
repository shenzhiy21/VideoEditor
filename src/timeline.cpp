#include "timeline.h"
#include "utils.h"
#include <QPainter>
#include <QPixmap>

const int imageTrackY = 40;
const int imageTrackHeight = 50;
const int videoTrackY = 105;
const int videoTrackHeight = Utils::singleRectHeight;
const int textTrackY = 180;
const int textTrackHeight = 35;

TimeLine::TimeLine(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(180 * Utils::lengthPerSec, 220);
    fps = Utils::fps;
    videoRank = 0;
    videoFrame = -1;
    currentTrack = Utils::FileType::NOTYPE;
}

void TimeLine::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 调整指针位置
        double xPos = event->position().x();
        double yPos = event->position().y();
        int futureFrame = xPos / Utils::lengthPerSec * Utils::fps;
        offset(futureFrame - currentFrame());

        changeTrack(yPos);
    }
}

void TimeLine::changeResourseState(int key)
{
    // WASD 按键可以调节素材的时长和位置

    if (key == Qt::Key_W)  // W: 时长加一帧
    {
        if (currentTrack == Utils::FileType::IMAGE)
        {
            for (auto& image : imagesInTimeLine)
            {
                if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
                {
                    image.endFrame++;
                    break;
                }
            }
        }
        else if (currentTrack == Utils::FileType::SUBTITLE)
        {
            for (auto& text : textsInTimeLine)
            {
                if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
                {
                    text.endFrame++;
                    break;
                }
            }
        }
    }
    else if (key == Qt::Key_A) // A: 前移一帧
    {
        if (currentTrack == Utils::FileType::IMAGE)
        {
            for (auto& image : imagesInTimeLine)
            {
                if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
                {
                    if (image.startFrame > 0)
                    {
                        image.startFrame--;
                        image.endFrame--;
                    }
                    break;
                }
            }
        }
        else if (currentTrack == Utils::FileType::SUBTITLE)
        {
            for (auto& text : textsInTimeLine)
            {
                if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
                {
                    if (text.startFrame > 0)
                    {
                        text.startFrame--;
                        text.endFrame--;
                    }
                    break;
                }
            }
        }
    }
    else if (key == Qt::Key_S) // S: 时长减一帧
    {
        if (currentTrack == Utils::FileType::IMAGE)
        {
            for (auto& image : imagesInTimeLine)
            {
                if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
                {
                    if (image.endFrame > image.startFrame) image.endFrame--;
                    break;
                }
            }
        }
        else if (currentTrack == Utils::FileType::SUBTITLE)
        {
            for (auto& text : textsInTimeLine)
            {
                if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
                {
                    if (text.endFrame > text.startFrame) text.endFrame--;
                    break;
                }
            }
        }
    }
    else if (key == Qt::Key_D) // D: 后移一帧
    {
        if (currentTrack == Utils::FileType::IMAGE)
        {
            for (auto& image : imagesInTimeLine)
            {
                if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
                {
                    image.startFrame++;
                    image.endFrame++;
                    break;
                }
            }
        }
        else if (currentTrack == Utils::FileType::SUBTITLE)
        {
            for (auto& text : textsInTimeLine)
            {
                if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
                {
                    text.startFrame++;
                    text.endFrame++;
                    break;
                }
            }
        }
    }
    if (videoFrame >= 0)
    {
        int curFrame = currentFrame();
        const auto& texts = getTexts(curFrame);
        const auto& images = getImages(curFrame);
        emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
    }
    this->update();
}

void TimeLine::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    // 画背景
    painter.setPen(QPen(Qt::gray, 1));
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(0, 0, 180 * Utils::lengthPerSec, 220);

    // 画当前轨道
    painter.setBrush(QBrush(Qt::white));
    switch(currentTrack)
    {
    case Utils::FileType::NOTYPE:
        break;
    case Utils::FileType::IMAGE:
        painter.drawRect(0, imageTrackY, 180 * Utils::lengthPerSec, imageTrackHeight);
        break;
    case Utils::FileType::VIDEO:
        painter.drawRect(0, imageTrackY + imageTrackHeight, 180 * Utils::lengthPerSec, textTrackY - imageTrackY - imageTrackHeight);
        break;
    case Utils::FileType::SUBTITLE:
        painter.drawRect(0, textTrackY, 180 * Utils::lengthPerSec, textTrackHeight);
        break;
    }

    // 画时间线
    for (int j = 0; j < 1800; ++j)
    {
        if (j % 10 == 0)
        {
            int i = j / 10;
            int height = (i % 10 == 0) ? 10 : 5;
            painter.drawLine(QPoint(Utils::lengthPerSec * i, 30), QPoint(Utils::lengthPerSec * i, 30 - height));

            int min = i / 60;
            int sec = i - min * 60;
            QString secText = (sec < 10) ? ("0" + QString::number(sec)) : QString::number(sec);
            QString text = QString::number(min) + ":" + secText;
            painter.drawText(Utils::lengthPerSec * i - 8, 17, text);
        }
        else
        {
            painter.drawLine(QPoint(Utils::lengthPerSec * j / 10, 30),
                             QPoint(Utils::lengthPerSec * j / 10, 28));
        }
    }

    // 画视频
    double curX = 0;
    for (const auto& video : videosInTimeLine)
    {
        double videoLength = video.duration * Utils::lengthPerSec;
        int frameCount = videoLength / Utils::singleRectWidth;
        double remainedWidth = videoLength - Utils::singleRectWidth * frameCount;
        for (int i = 0; i < frameCount; ++i)
        {
            QPixmap pixmap = QPixmap::fromImage(video.images[i * Utils::secForSingleRect * Utils::fps]);
            QRectF rect(curX, videoTrackY, Utils::singleRectWidth, videoTrackHeight);
            painter.drawPixmap(rect, pixmap, QRectF(0, 0, pixmap.width(), pixmap.height()));
            curX += Utils::singleRectWidth;
        }
        if (remainedWidth > 0.001)
        {
            QPixmap pixmap = QPixmap::fromImage(video.images[frameCount * Utils::secForSingleRect * Utils::fps]);
            QRectF rect(curX, videoTrackY, remainedWidth, videoTrackHeight);
            painter.drawPixmap(rect, pixmap, QRectF(0, 0,
                remainedWidth / Utils::singleRectHeight * pixmap.height(), pixmap.height()));
            curX += remainedWidth;
        }
    }

    // 画包围视频的矩形框
    painter.setPen(QPen(QColor(0, 77, 82), 1));
    painter.setBrush(QBrush(QColor(0, 77, 82)));
    curX = 0;
    for (int i = 0; i < videosInTimeLine.size(); ++i)
    {
        double length = videosInTimeLine[i].duration * Utils::lengthPerSec;
        if (i != videoRank)
        {
            painter.drawRoundedRect(curX, 95, length, 10, 5, 5);
            painter.drawRoundedRect(curX, 165, length, 10, 5, 5);
            curX += length;
        }
        else
        {
            painter.setBrush(QBrush(QColor(189, 252, 201)));
            painter.drawRoundedRect(curX, 95, length, 10, 5, 5);
            painter.drawRoundedRect(curX, 165, length, 10, 5, 5);
            curX += length;
            painter.setBrush(QBrush(QColor(0, 77, 82)));
        }
    }

    // 画图片素材
    painter.setBrush(QBrush(QColor(135, 202, 245)));
    for (const auto& image : imagesInTimeLine)
    {
        QRectF rect((double)image.startFrame / Utils::fps * Utils::lengthPerSec, imageTrackY,
                    (double)(image.endFrame - image.startFrame) / Utils::fps * Utils::lengthPerSec,
                    imageTrackHeight);
        if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
        {
            painter.setBrush(QBrush(QColor(91, 176, 251)));
            painter.drawRoundedRect(rect, 5, 5);
            painter.setBrush(QBrush(QColor(135, 202, 245)));
        }
        else
        {
            painter.drawRoundedRect(rect, 5, 5);
        }
    }

    // 画文字素材
    for (const auto& text : textsInTimeLine)
    {
        QRectF rect((double)text.startFrame / Utils::fps * Utils::lengthPerSec, textTrackY,
                    (double)(text.endFrame - text.startFrame) / Utils::fps * Utils::lengthPerSec, textTrackHeight);
        if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
        {
            painter.setBrush(QBrush(QColor(91, 176, 251)));
            painter.drawRoundedRect(rect, 5, 5);
            painter.setBrush(QBrush(QColor(135, 202, 245)));
        }
        else
        {
            painter.setBrush(QBrush(QColor(135, 202, 245)));
            painter.drawRoundedRect(rect, 5, 5);
        }

        painter.setPen(QPen(Qt::black));
        painter.drawText(rect, text.text);
    }

    // 画指针
    int frame = 0;
    for (int i = 0; i < videoRank; ++i)
    {
        frame += videosInTimeLine[i].images.size();
    }
    frame += videoFrame;
    double length = frame * Utils::lengthPerSec / Utils::fps;
    painter.setPen(QPen(Qt::white, 3));
    painter.drawLine(length, 0, length, 220);
    painter.drawRect(length - 2, 0, 4, 5);
}

void TimeLine::addNewVideo(const QString& path)
{
    videosInTimeLine.push_back(VideoTimeLine(path, Utils::fps));
    this->update();
    emit timeLineChanged(totalFrames());
}

void TimeLine::updateVideo()
{
    if (videosInTimeLine.size() == 0) return;
    if (videoRank >= videosInTimeLine.size())
    {
        videoRank = 0;
        videoFrame = -1;
        return;
    }

    if (videoFrame >= videosInTimeLine[videoRank].images.size() - 1)
    {
        videoRank++;
        videoFrame = 0;
        videoRank %= videosInTimeLine.size();
    }
    else
    {
        videoFrame++;
    }
    this->update();
    int curFrame = currentFrame();
    const auto& texts = getTexts(curFrame);
    const auto& images = getImages(curFrame);
    emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
    emit currentFrameChanged(currentFrame());
}

int TimeLine::totalFrames() const
{
    int ret = 0;
    for (auto& video : videosInTimeLine)
    {
        ret += video.images.size();
    }
    return ret;
}

int TimeLine::currentFrame() const
{
    int ret = 0;
    for (int i = 0; i < videoRank; ++i)
    {
        ret += videosInTimeLine[i].images.size();
    }
    ret += videoFrame;
    return ret;
}

int TimeLine::getFrames(const int rank, const int frame) const
{
    int ret = 0;
    for (int i = 0; i < rank; ++i)
    {
        ret += videosInTimeLine[i].images.size();
    }
    ret += frame;
    return ret;
}

void TimeLine::setFrame(int value)
{
    if (value >= totalFrames()) value = totalFrames() - 1;
    else if (value < 0) value = 0;
    for (int i = 0; i < videosInTimeLine.size(); ++i)
    {
        if (value < videosInTimeLine[i].images.size())
        {
            videoRank = i;
            videoFrame = value;
            this->update();
            int curFrame = currentFrame();
            const auto& texts = getTexts(curFrame);
            const auto& images = getImages(curFrame);
            emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
            emit currentFrameChanged(currentFrame());
            break;
        }
        else
        {
            value -= videosInTimeLine[i].images.size();
        }
    }
}

bool TimeLine::isValid() const
{
    // videoRank 过大或过小
    if (videoRank >= videosInTimeLine.size() || videoRank < 0) return false;
    return true;
}

void TimeLine::cutVideoByFrame()
{
    // 排除特殊情况
    if (!isValid()) return;
    // 刚好在某个视频的第一帧或最后一帧，则不剪切
    if (videoFrame >= videosInTimeLine[videoRank].images.size() - 1 || videoFrame <= 0) return;

    // 开始剪切
    // 删除原视频
    auto oldVideo = videosInTimeLine.takeAt(videoRank);
    // 获得剪切后的两个新视频
    auto newVideo1 = VideoTimeLine(oldVideo, 0, videoFrame);
    auto newVideo2 = VideoTimeLine(oldVideo, videoFrame, oldVideo.images.size());
    // 添加新视频
    videosInTimeLine.insert(videoRank, newVideo2);
    videosInTimeLine.insert(videoRank, newVideo1);
    // 修改指针位置：将其置于剪切出的后一个视频的第一帧
    videoRank++;
    videoFrame = 0;
    // 重绘时间轴
    this->update();
}

bool TimeLine::hasVideos() const
{
    if (videosInTimeLine.size() > 0) return true;
    return false;
}

void TimeLine::merge()
{
    // 排除特殊情况
    if (!isValid()) return;
    if (videosInTimeLine.size() <= 1) return;   // 只有一个视频，没法合并

    // 开始合并
    if (videoRank == 0)
    {
        mergeTwoVideos(0, 1);
    }
    else if (videoRank == videosInTimeLine.size() - 1)
    {
        mergeTwoVideos(videoRank-1, videoRank);
    }
    else    // 当前视频是中间位置的某个视频
    {
        // 如果指针在该视频偏左的位置，就跟左边的视频合并
        if (videoFrame * 2 < videosInTimeLine[videoRank].images.size())
        {
            mergeTwoVideos(videoRank-1, videoRank);
        }
        else    // 否则跟右边视频合并
        {
            mergeTwoVideos(videoRank, videoRank+1);
        }
    }
}

void TimeLine::mergeTwoVideos(int first, int second)
{
    // assert: second == first + 1，也就是说这两个视频是相邻的
    if (second - first != 1)
    {
        qDebug() << "Merge videos: index error!";
        return;
    }

    // 合并视频
    qDebug() << "merge start!";
    auto oldVideo = videosInTimeLine.takeAt(second);
    videosInTimeLine[first].mergeWith(oldVideo);

    // 更新指针位置：重置到合并后的视频的第一帧
    videoFrame = 0;
    videoRank = first;

    // 重绘时间轴
    this->update();
    int curFrame = currentFrame();
    const auto& texts = getTexts(curFrame);
    const auto& images = getImages(curFrame);
    emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
    emit currentFrameChanged(currentFrame());
}

void TimeLine::deleteSingleResource()
{
    // 排除特殊情况
    if (!isValid()) return;

    // 删除
    if (currentTrack == Utils::FileType::VIDEO)
    {
        videosInTimeLine.removeAt(videoRank);

        // 修改指针位置
        if (videosInTimeLine.size() <= 0)
        {
            videoRank = 0;
            videoFrame = -1;
        }
        else
        {
            videoFrame = 0;
            videoRank--;
            int curFrame = currentFrame();
            const auto& texts = getTexts(curFrame);
            const auto& images = getImages(curFrame);
            emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
            emit currentFrameChanged(currentFrame());
            emit timeLineChanged(totalFrames());
            this->update();
        }
    }
    else if (currentTrack == Utils::FileType::IMAGE)
    {
        for (int i = 0; i < imagesInTimeLine.size(); ++i)
        {
            auto image = imagesInTimeLine[i];
            if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
            {
                imagesInTimeLine.removeAt(i);
                int curFrame = currentFrame();
                const auto& texts = getTexts(curFrame);
                const auto& images = getImages(curFrame);
                emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
                this->update();
                break;
            }
        }
    }
    else if (currentTrack == Utils::FileType::SUBTITLE)
    {
        for (int i = 0; i < textsInTimeLine.size(); ++i)
        {
            auto text = textsInTimeLine[i];
            if (currentFrame() >= text.startFrame && currentFrame() <= text.endFrame)
            {
                textsInTimeLine.removeAt(i);
                int curFrame = currentFrame();
                const auto& texts = getTexts(curFrame);
                const auto& images = getImages(curFrame);
                emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
                this->update();
                break;
            }
        }
    }
    else return;
}

void TimeLine::swap(int offset)
{
    // 排除特殊情况
    if (!isValid()) return;
    if (videosInTimeLine.size() <= 1) return;
    if (offset == 0) return;
    int pos = videoRank + offset;
    if ((pos < 0) || (pos >= videosInTimeLine.size())) return;

    // 开始交换顺序
    std::swap(videosInTimeLine[videoRank], videosInTimeLine[pos]);

    // 修改指针位置
    videoRank = pos;

    // 重绘时间轴
    this->update();
}

int TimeLine::setToBegin()
{
    if (!isValid()) return 0;

    videoFrame = 0;
    videoRank = 0;
    this->update();
    return 0;
}

int TimeLine::setToEnd()
{
    if (!isValid()) return 0;

    videoRank = videosInTimeLine.size() - 1;
    videoFrame = videosInTimeLine[videoRank].images.size() - 1;
    this->update();
    return totalFrames() - 1;
}

void TimeLine::offset(int val)
{
    // 排除特殊情况
    if (!isValid()) return;

    // 偏移后若超出时间轴范围，则return
    int nowValue = currentFrame() + val;
    if ((nowValue < 0) || (nowValue >= totalFrames())) return;

    // 进行操作
    this->setFrame(nowValue);
}

void TimeLine::addImage(const QRect& rect, const QString& path)
{
    ImageTimeLine image = ImageTimeLine(rect, path);

    // 对于拖进来的图片，默认将其显示的时间范围设成当前视频的时长的一半
    int curFrame = currentFrame();
    image.startFrame = curFrame - Utils::fps;
    if (image.startFrame < 0) image.startFrame = 0;
    image.endFrame = curFrame + Utils::fps;
    imagesInTimeLine.push_back(image);
    if (videoFrame >= 0)
    {
        int curFrame = currentFrame();
        const auto& texts = getTexts(curFrame);
        const auto& images = getImages(curFrame);
        emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
    }
    this->updateVideo();
}

void TimeLine::addImageByDefault(const QString& path)
{
    QImage img;
    bool ret = img.load(path);
    if (!ret)
    {
        qDebug() << "TimeLine: Image Load Fail!";
        return;
    }
    QRect rect(0, 0, img.width(), img.height());
    addImage(rect, path);
}

QList<ImageTimeLine> TimeLine::getImages(const int curFrame) const
{
    QList<ImageTimeLine> ret;
    for (const auto& image : imagesInTimeLine)
    {
        if (curFrame >= image.startFrame && curFrame <= image.endFrame)
        {
            ret.push_back(image);
        }
    }
    return ret;
}

void TimeLine::addSubtitle(const SubtitleProcesser& subtitleProcesser)
{
    for (const auto& subtitle : subtitleProcesser.subtitles)
    {
        TextTimeLine text(QString::fromStdString(subtitle.text),
                          subtitle.startTime * Utils::fps / 1000, subtitle.endTime * Utils::fps / 1000);
        text.rect = QRect(0, 200, 400, 25);
        textsInTimeLine.push_back(text);
    }
    if (videoFrame >= 0)
    {
        int curFrame = currentFrame();
        const auto& texts = getTexts(curFrame);
        const auto& images = getImages(curFrame);
        emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
    }
    this->update();
}

QList<TextTimeLine> TimeLine::getTexts(const int curFrame) const
{
    QList<TextTimeLine> ret;
    for (const auto& text : textsInTimeLine)
    {
        if (curFrame >= text.startFrame && curFrame <= text.endFrame)
        {
            ret.push_back(text);
        }
    }
    return ret;
}

void TimeLine::receiveText(TextTimeLine text)
{
    if (!isValid()) return;
    int curFrame = currentFrame();
    int startFrame = curFrame - Utils::fps;
    if (startFrame < 0) startFrame = 0;
    int endFrame = startFrame + 2 * Utils::fps;
    text.startFrame = startFrame;
    text.endFrame = endFrame;

    textsInTimeLine.push_back(text);
    const auto& texts = getTexts(curFrame);
    const auto& images = getImages(curFrame);
    emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
}

void TimeLine::replaceText(TextTimeLine text)
{
    if (!isValid()) return;

    for (auto& oldText : textsInTimeLine)
    {
        if (oldText.text == text.text)
        {
            if (oldText.color == text.color && oldText.font == text.font)
            {
                oldText.rect = text.rect;
                break;
            }
            else continue;
        }
    }

    int curFrame = currentFrame();
    const auto& texts = getTexts(curFrame);
    const auto& images = getImages(curFrame);
    emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
}

void TimeLine::changeTrack(int yPos)
{
    if (yPos >= imageTrackY && yPos <= imageTrackY + imageTrackHeight)
    {
        currentTrack = Utils::FileType::IMAGE;
    }
    else if (yPos >= videoTrackY && yPos <= videoTrackY + videoTrackHeight)
    {
        currentTrack = Utils::FileType::VIDEO;
    }
    else if (yPos >= textTrackY && yPos <= textTrackY + textTrackHeight)
    {
        currentTrack = Utils::FileType::SUBTITLE;
    }
    else
    {
        currentTrack = Utils::FileType::NOTYPE;
    }
    this->update();
}

void TimeLine::tryFade(int fadeInTime, int fadeOutTime)
{
    if (!isValid()) return;

    if (currentTrack == Utils::FileType::IMAGE)
    {
        for (auto& image : imagesInTimeLine)
        {
            if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
            {
                image.isFade = true;
                image.fadeInFrames = fadeInTime * Utils::fps / 1000;
                image.fadeOutFrames = fadeOutTime * Utils::fps / 1000;
                int curFrame = currentFrame();
                const auto& texts = getTexts(curFrame);
                const auto& images = getImages(curFrame);
                emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
                break;
            }
        }
    }
    else return;
}

void TimeLine::tryMove(int distance)
{
    if (!isValid()) return;

    if (currentTrack == Utils::FileType::IMAGE)
    {
        for (auto& image : imagesInTimeLine)
        {
            if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
            {
                image.isMove = true;
                image.moveDistance = distance;

                int curFrame = currentFrame();
                const auto& texts = getTexts(curFrame);
                const auto& images = getImages(curFrame);
                emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
                break;
            }
        }
    }
    else return;
}

void TimeLine::tryScale(double ratio)
{
    if (!isValid()) return;

    if (currentTrack == Utils::FileType::IMAGE)
    {
        for (auto& image : imagesInTimeLine)
        {
            if (currentFrame() >= image.startFrame && currentFrame() <= image.endFrame)
            {
                image.isScale = true;
                image.scaleRatio = ratio;

                int curFrame = currentFrame();
                const auto& texts = getTexts(curFrame);
                const auto& images = getImages(curFrame);
                emit imageChanged(curFrame, videosInTimeLine[videoRank].images[videoFrame], images, texts);
                break;
            }
        }
    }
    else return;
}
