#include "mainwindow.h"
#include "texteditdialog.h"
#include "utils.h"
#include "ffmpeg.h"
#include "previewwindow.h"
#include "videocutdialog.h"
#include "fadeeffectdialog.h"
#include "moveeffectdialog.h"
#include "sizeeffectdialog.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAudioOutput>
#include <QProcess>
#include <QPushButton>
#include <QScrollBar>
#include <QProgressBar>
#include <QKeyEvent>
#include <QDrag>
#include <QPainter>
#include <QMimeData>
using namespace Utils;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("VideoEditor");

    // 设置窗口大小固定
    this->setFixedSize(800, 600);

    // 生成临时文件夹，用于存放中间信息（视频、图像等）
    dir = new QTemporaryDir();
    qDebug() << dir->path();
    bufferDir = dir->filePath("Buffer");
    QDir qDir;
    qDir.mkdir(bufferDir);

    fileSelector = new QFileDialog(this);
    fileSelector->setFileMode(QFileDialog::ExistingFiles);

    connect(ui->inputMenu, SIGNAL(triggered()), this, SLOT(selectFiles()));
    connect(ui->inputBtn, SIGNAL(clicked()), this, SLOT(selectFiles()));

    connect(ui->outputMenu, SIGNAL(triggered()), this, SLOT(outputVideo()));

    connect(ui->removeBtn, SIGNAL(clicked()), this, SLOT(removeFiles()));
    connect(ui->delMenu, SIGNAL(triggered()), this, SLOT(removeFiles()));

    connect(ui->renameBtn, SIGNAL(clicked()), this, SLOT(renameFiles()));
    connect(ui->renameMenu, SIGNAL(triggered()), this, SLOT(renameFiles()));

    connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(addResource()));
    connect(ui->addMenu, SIGNAL(triggered()), this, SLOT(addResource()));

    connect(ui->cutBtn, SIGNAL(clicked()), this, SLOT(cutVideo()));
    connect(ui->splitMenu, SIGNAL(triggered()), this, SLOT(cutVideo()));

    connect(ui->mergeBtn, SIGNAL(clicked()), this, SLOT(mergeVideos()));
    connect(ui->mergeMenu, SIGNAL(triggered()), this, SLOT(mergeVideos()));

    connect(ui->delBtn, SIGNAL(clicked()), this, SLOT(deleteResource()));
    connect(ui->rmvMenu, SIGNAL(triggered()), this, SLOT(deleteResource()));

    connect(ui->forwardBtn, SIGNAL(clicked()), this, SLOT(forwardSwap()));
    connect(ui->forwardMenu, SIGNAL(triggered()), this, SLOT(forwardSwap()));

    connect(ui->backwardBtn, SIGNAL(clicked()), this, SLOT(backwardSwap()));
    connect(ui->backwardMenu, SIGNAL(triggered()), this, SLOT(backwardSwap()));

    connect(ui->videoCutMenu, SIGNAL(triggered()), this, SLOT(showVideoCut()));

    connect(ui->toBeginBtn, SIGNAL(clicked()), this, SLOT(toBegin()));
    connect(ui->toBeginMenu, SIGNAL(triggered()), this, SLOT(toBegin()));

    connect(ui->toEndBtn, SIGNAL(clicked()), this, SLOT(toEnd()));
    connect(ui->toEndMenu, SIGNAL(triggered()), this, SLOT(toEnd()));

    connect(ui->backwardFastBtn, SIGNAL(clicked()), this, SLOT(oneFrameBefore()));
    connect(ui->stepbackMenu, SIGNAL(triggered()), this, SLOT(oneFrameBefore()));

    connect(ui->forwardFastBtn, SIGNAL(clicked()), this, SLOT(oneFrameAfter()));
    connect(ui->stepforthMenu, SIGNAL(triggered()), this, SLOT(oneFrameAfter()));

    connect(ui->playpauseBtn, SIGNAL(clicked()), this, SLOT(playpauseAction()));
    connect(ui->playpauseMenu, SIGNAL(triggered()), this, SLOT(playpauseAction()));

    connect(ui->imageList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(createQDrag(QListWidgetItem*)));

    connect(ui->previewWin, SIGNAL(imageToAdd(QRect, QString)), ui->timeLine, SLOT(addImage(QRect, QString)));

    connect(ui->fadeMenu, SIGNAL(triggered()), this, SLOT(showFadeDialog()));

    connect(ui->moveMenu, SIGNAL(triggered()), this, SLOT(showMoveDialog()));

    connect(ui->sizeMenu, SIGNAL(triggered()), this, SLOT(showSizeDialog()));

    ui->videoList->setIconSize(QSize(100, 100));
    ui->imageList->setIconSize(QSize(100, 100));
    ui->subtitleList->setIconSize(QSize(100, 100));

    // 同步时间轴和预览窗口
    timer = new QTimer(this);
    timer->setInterval(1000 / Utils::fps);
    connect(timer, SIGNAL(timeout()), ui->timeLine, SLOT(updateVideo()));
    connect(ui->timeLine, SIGNAL(imageChanged(int, QImage, QList<ImageTimeLine>, QList<TextTimeLine>)),
            ui->previewWin, SLOT(showImage(int, QImage, QList<ImageTimeLine>, QList<TextTimeLine>)));
    connect(ui->timeLine, SIGNAL(timeLineChanged(int)), ui->slider, SLOT(setTotalFrame(int)));
    connect(ui->timeLine, SIGNAL(currentFrameChanged(int)), ui->slider, SLOT(setCurrentFrame(int)));
    connect(ui->slider, SIGNAL(mousePress(int)), this, SLOT(sliderPressEffect(int)));
    connect(ui->slider, SIGNAL(mouseRelease(int)), this, SLOT(sliderReleaseEffect(int)));
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoveEffect(int)));
    connect(ui->previewWin, SIGNAL(textToLoad(TextTimeLine)), ui->timeLine, SLOT(receiveText(TextTimeLine)));
    connect(ui->previewWin, SIGNAL(textToReplace(TextTimeLine)), ui->timeLine, SLOT(replaceText(TextTimeLine)));

    // 设置预览窗口属性
    isPlay = false;

    // 设置按钮图标
    Utils::mySetIcon(ui->inputBtn, ":/res/cart-plus.svg");
    Utils::mySetIcon(ui->renameBtn, ":/res/retweet.svg");
    Utils::mySetIcon(ui->removeBtn, ":/res/trash-can-arrow-up.svg");
    Utils::mySetIcon(ui->addBtn, ":/res/plus.svg");
    Utils::mySetIcon(ui->cutBtn, ":/res/scissors.svg");
    Utils::mySetIcon(ui->mergeBtn, ":/res/code-merge.svg");
    Utils::mySetIcon(ui->delBtn, ":/res/trash-can.svg");
    Utils::mySetIcon(ui->forwardBtn, ":/res/arrow-left.svg");
    Utils::mySetIcon(ui->backwardBtn, ":/res/arrow-right.svg");
    Utils::mySetIcon(ui->toBeginBtn, ":/res/backward-step.svg");
    Utils::mySetIcon(ui->toEndBtn, ":/res/forward-step.svg");
    Utils::mySetIcon(ui->backwardFastBtn, ":/res/backward.svg");
    Utils::mySetIcon(ui->forwardFastBtn, ":/res/forward.svg");
    ui->playpauseBtn->setIcons(":/res/play.svg", ":/res/pause.svg");

    // 初始化时间显示标签
    ui->timeLabel->setText("00:00/00:00");
}

MainWindow::~MainWindow()
{
    // 删除临时文件夹
    delete dir;

    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // "Space" 快捷键：可以控制播放器播放与暂停的状态
    if (event->key() == Qt::Key_Space)
    {
        if (ui->timeLine->videosInTimeLine.size() == 0) return;
        if (!isPlay)
        {
            isPlay = true;
            this->startPlayVideo();
        }
        else
        {
            isPlay = false;
            this->stopPlayVideo();
        }
        ui->playpauseBtn->changeIcon();
    }
    // "WASD" 键：可以改变时间轴中素材的时长和时间位置
    else if (event->key() == Qt::Key_A || event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_D)
    {
        ui->timeLine->changeResourseState(event->key());
    }
}

void MainWindow::selectFiles()
{
    fileSelector->show();

    // 设置可以选择的文件格式（视频 / 图片 / 字幕）
    QString validFileType;

    int filetype = this->fileType();
    switch (filetype)
    {
    case Utils::FileType::VIDEO:
        validFileType = tr("Videos (*.mp4 *.mkv *.avi)");
        break;
    case Utils::FileType::IMAGE:
        validFileType = tr("Images (*.png *.xpm *.jpg *.jfif)");
        break;
    case Utils::FileType::SUBTITLE:
        validFileType = tr("Subtitles (*.ass *.vtt *.srt)");
        break;
    default:
        break;
    }

    fileSelector->setNameFilter(validFileType);

    // 导入文件并处理
    QStringList fileNames;
    if (fileSelector->exec())
    {
        fileNames = fileSelector->selectedFiles();
        qDebug() << "load files:" << fileNames;

        if (filetype == Utils::FileType::VIDEO)
        {
            for (qsizetype i = 0; i < fileNames.size(); ++i)
            {
                this->addNewVideo(fileNames[i]);
            }
        }
        else if (filetype == Utils::FileType::IMAGE)
        {
            for (qsizetype i = 0; i < fileNames.size(); ++i)
            {
                ImageView img(fileNames[i]);
                imageList.push_back(img);
                QListWidgetItem* item = new QListWidgetItem(ui->imageList);
                item->setText(img.name);
                item->setIcon(img.pixmap);
                ui->imageList->addItem(item);
            }
        }
        else if (filetype == Utils::FileType::SUBTITLE)
        {
            for (qsizetype i = 0; i < fileNames.size(); ++i)
            {
                SubtitleProcesser processer(fileNames[i].toStdString());
                bool ret = processer.process();
                if (!ret)
                {
                    qDebug() << "Subtitle Format Fail";
                    continue;
                }
                QListWidgetItem* item = new QListWidgetItem(ui->subtitleList);
                subtitleList.push_back(processer);
                item->setText(processer.name);
                ui->subtitleList->addItem(item);
            }
        }
    }
}

void MainWindow::renameFiles()
{
    QListWidget* listWidget;
    if (ui->tabWidget->currentIndex() == 0) listWidget = ui->videoList;
    else if (ui->tabWidget->currentIndex() == 1) listWidget = ui->imageList;
    else if (ui->tabWidget->currentIndex() == 2) listWidget = ui->subtitleList;
    else return;

    if (listWidget->currentRow() < 0) return;
    TextEditDialog* dialog = new TextEditDialog(listWidget->currentItem()->text(), false, this);
    dialog->show();
    connect(dialog, SIGNAL(renameSignal(QString)), this, SLOT(changeName(QString)));
}

void MainWindow::changeName(QString name)
{
    // *** 注意：我们不允许 QListWidget 中有两个重名的 item ***
    QListWidget* listWidget;
    if (ui->tabWidget->currentIndex() == 0) listWidget = ui->videoList;
    else if (ui->tabWidget->currentIndex() == 1) listWidget = ui->imageList;
    else listWidget = ui->subtitleList;

    QString originalName = listWidget->currentItem()->text();
    listWidget->currentItem()->setText(name);
    if (fileType() == Utils::FileType::VIDEO)
    {
        for (auto iter = videoList.begin(); iter != videoList.end(); ++iter)
        {
            if (iter->name == originalName)
            {
                iter->name = name;
                break;
            }
        }
    }
    else if (fileType() == Utils::FileType::IMAGE)
    {
        for (auto iter = imageList.begin(); iter != imageList.end(); ++iter)
        {
            if (iter->name == originalName)
            {
                iter->name = name;
                break;
            }
        }
    }
    else if (fileType() == Utils::FileType::SUBTITLE)
    {
        for (auto iter = subtitleList.begin(); iter != subtitleList.end(); ++iter)
        {
            if (iter->name == originalName)
            {
                iter->name = name;
                break;
            }
        }
    }
}

void MainWindow::removeFiles()
{
    QListWidget* listWidget;
    if (ui->tabWidget->currentIndex() == 0) listWidget = ui->videoList;
    else if (ui->tabWidget->currentIndex() == 1) listWidget = ui->imageList;
    else if (ui->tabWidget->currentIndex() == 2) listWidget = ui->subtitleList;
    else return;

    if (listWidget->count() == 0) return;

    QString text = listWidget->currentItem()->text();
    listWidget->takeItem(listWidget->row(listWidget->currentItem()));
    if (fileType() == Utils::FileType::VIDEO)
    {
        for (auto iter = videoList.begin(); iter != videoList.end(); ++iter)
        {
            if (iter->name == text)
            {
                iter->deleteBuffer();
                videoList.erase(iter);
                break;
            }
        }
    }
    else if (fileType() == Utils::FileType::IMAGE)
    {
        for (auto iter = imageList.begin(); iter != imageList.end(); ++iter)
        {
            if (iter->name == text)
            {
                imageList.erase(iter);
                break;
            }
        }
    }
    else if (fileType() == Utils::FileType::SUBTITLE)
    {
        for (auto iter = subtitleList.begin(); iter != subtitleList.end(); ++iter)
        {
            if (iter->name == text)
            {
                subtitleList.erase(iter);
                break;
            }
        }
    }
}

int MainWindow::fileType() const
{
    int filetype = Utils::FileType::NOTYPE;
    if (ui->tabWidget->currentIndex() == 0) filetype = Utils::FileType::VIDEO;
    else if (ui->tabWidget->currentIndex() == 1) filetype = Utils::FileType::IMAGE;
    else if (ui->tabWidget->currentIndex() == 2) filetype = Utils::FileType::SUBTITLE;
    return filetype;
}

void MainWindow::showVideoCut()
{
    auto video = selectVideo();
    if (!video.isValid) return;
    VideoCutDialog* dialog = new VideoCutDialog(video.path, this);
    dialog->show();
    connect(dialog, SIGNAL(cutVideoMessage(const QString&)), this, SLOT(addNewVideo(const QString&)));
}

void MainWindow::addNewVideo(const QString& path)
{
    VideoView video(dir->path(), path, Utils::fps);
    videoList.push_back(video);

    QListWidgetItem* item = new QListWidgetItem(ui->videoList);
    item->setText(video.name);
    item->setIcon(video.pixmap);
    ui->videoList->addItem(item);
}

const VideoView MainWindow::selectVideo() const
{
    if ((ui->videoList->currentRow() < 0) || (fileType() != Utils::FileType::VIDEO)) return VideoView();
    for (const auto& video : videoList)
    {
        if (video.name == ui->videoList->currentItem()->text())
        {
            return video;
        }
    }
    return VideoView();
}

const ImageView MainWindow::selectImage() const
{
    if ((ui->imageList->currentRow() < 0) || (fileType() != Utils::FileType::IMAGE)) return ImageView();
    for (const auto& image : imageList)
    {
        if (image.name == ui->imageList->currentItem()->text())
        {
            return image;
        }
    }
    return ImageView();
}

const SubtitleProcesser MainWindow::selectSubtitles() const
{
    if ((ui->subtitleList->currentRow() < 0) || (fileType() != Utils::FileType::SUBTITLE)) return SubtitleProcesser();
    for (const auto& subtitle : subtitleList)
    {
        if (subtitle.name == ui->subtitleList->currentItem()->text())
        {
            return subtitle;
        }
    }
    return SubtitleProcesser();
}

void MainWindow::addResource()
{
    int filetype = fileType();
    if (filetype == Utils::FileType::VIDEO)
    {
        const auto& video = selectVideo();
        if (!video.isValid) return;
        ui->timeLine->addNewVideo(video.buffer);
    }
    else if (filetype == Utils::FileType::IMAGE)
    {
        const auto& image = selectImage();
        if (!image.isValid) return;
        ui->timeLine->addImageByDefault(image.path);
    }
    else if (filetype == Utils::FileType::SUBTITLE)
    {
        const auto& subtitle = selectSubtitles();
        if (!subtitle.isValid) return;
        ui->timeLine->addSubtitle(subtitle);
    }
}

void MainWindow::sliderPressEffect(int value)
{
    this->timer->stop();
    if (!ui->playpauseBtn->isFirstIcon)
    {
        ui->playpauseBtn->changeIcon();
    }
    qDebug() << "mouse pressed:" << value;
}

void MainWindow::sliderReleaseEffect(int value)
{
    ui->timeLine->setFrame(value);
    this->timer->start();
    if (ui->playpauseBtn->isFirstIcon)
    {
        ui->playpauseBtn->changeIcon();
    }
    isPlay = true;
    qDebug() << "mouse released:" << value;
}

void MainWindow::sliderMoveEffect(int value)
{
    ui->timeLine->setFrame(value);
    int totalTime = ui->timeLine->totalFrames() / Utils::fps;
    int curTime = ui->timeLine->currentFrame() / Utils::fps;
    QString curText = Utils::calculateThirdTime(curTime);
    QString totalText = Utils::calculateThirdTime(totalTime);
    ui->timeLabel->setText((curText + "/" + totalText));
}

void MainWindow::cutVideo()
{
    stopPlayVideo();
    ui->timeLine->cutVideoByFrame();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
}

void MainWindow::startPlayVideo()
{
    if (!ui->timeLine->hasVideos()) return;
    isPlay = true;
    this->timer->start();
}

void MainWindow::stopPlayVideo()
{
    if (!ui->timeLine->hasVideos()) return;
    isPlay = false;
    this->timer->stop();
}

void MainWindow::playpauseAction()
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    if (ui->playpauseBtn->isFirstIcon)
    {
        startPlayVideo();
    }
    else
    {
        stopPlayVideo();
    }
    ui->playpauseBtn->changeIcon();
}

void MainWindow::mergeVideos()
{
    stopPlayVideo();
    ui->timeLine->merge();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
}

void MainWindow::deleteResource()
{
    stopPlayVideo();
    ui->timeLine->deleteSingleResource();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
}

void MainWindow::forwardSwap()
{
    stopPlayVideo();
    ui->timeLine->swap(-1);
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
}

void MainWindow::backwardSwap()
{
    stopPlayVideo();
    ui->timeLine->swap(1);
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
}

void MainWindow::toBegin()
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    stopPlayVideo();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
    int val = ui->timeLine->setToBegin();
    ui->slider->setValue(val);
}

void MainWindow::toEnd()
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    stopPlayVideo();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
    int val = ui->timeLine->setToEnd();
    ui->slider->setValue(val);
}

void MainWindow::oneFrameBefore()
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    stopPlayVideo();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
    ui->timeLine->offset(-1);
}

void MainWindow::oneFrameAfter()
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    stopPlayVideo();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();
    ui->timeLine->offset(1);
}

void MainWindow::createQDrag(QListWidgetItem* item)
{
    if (ui->timeLine->videosInTimeLine.size() == 0) return;
    stopPlayVideo();
    if (!ui->playpauseBtn->isFirstIcon) ui->playpauseBtn->changeIcon();

    if (this->fileType() != Utils::FileType::IMAGE) return; // 只支持拖入图片素材

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap;

    for (const auto& image : imageList)
    {
        if (image.name == item->text())
        {
            pixmap = image.pixmap;
            dataStream << pixmap << image.path;
            break;
        }
    }

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/image", itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
    drag->setPixmap(pixmap);

    drag->exec();
}

void MainWindow::outputVideo()
{
    // TODO: 设置帧率之类的
    QDir qDir;
    tempDir = bufferDir + "\\TempDir";
    qDir.mkdir(tempDir);

    QString outputPath = QFileDialog::getSaveFileName(this, tr("导出路径"), tr("output"), tr("Videos (*.mp4 *.mkv *.avi)"));

    // 设置进度条
    QProgressBar* bar = new QProgressBar(this);
    bar->setRange(0, 100);
    bar->setFixedSize(200, 50);
    bar->move(this->width()/2 - 100, this->height()/2 - 25);
    bar->setValue(0);
    bar->setTextVisible(false);
    bar->show();

    // 从第一帧开始画
    int curRank = 0;
    int curFrame = 0;

    int totalFrame = ui->timeLine->totalFrames();
    for (int framesTillNow = 0; framesTillNow < totalFrame; ++framesTillNow)
    {
        QImage videoImg = ui->timeLine->videosInTimeLine[curRank].images[curFrame];
        QPainter painter(&videoImg);
        for (const auto& imageInTimeLine : ui->timeLine->imagesInTimeLine)
        {
            if (framesTillNow >= imageInTimeLine.startFrame && framesTillNow <= imageInTimeLine.endFrame)
            {
                QRect rect = imageInTimeLine.rect;
                QImage img = imageInTimeLine.image;
                if (imageInTimeLine.isFade)
                {
                    double opacity = 1;
                    if (curFrame - imageInTimeLine.startFrame < imageInTimeLine.fadeInFrames)
                    {
                        opacity = (double)(curFrame - imageInTimeLine.startFrame) / imageInTimeLine.fadeInFrames;
                    }
                    else if (imageInTimeLine.endFrame - curFrame < imageInTimeLine.fadeOutFrames)
                    {
                        opacity = (double)(imageInTimeLine.endFrame - curFrame) / imageInTimeLine.fadeOutFrames;
                    }
                    painter.setOpacity(opacity);
                }
                if (imageInTimeLine.isMove)
                {
                    int dx = (double)(imageInTimeLine.endFrame - curFrame) * imageInTimeLine.moveDistance
                            / (imageInTimeLine.endFrame - imageInTimeLine.startFrame);
                    rect.moveTo(imageInTimeLine.rect.x()+dx, imageInTimeLine.rect.y());
                }
                if (imageInTimeLine.isScale)
                {
                    double ratio = (double)(curFrame - imageInTimeLine.startFrame)
                            * imageInTimeLine.scaleRatio / imageInTimeLine.endFrame;
                    double curScaledRatio = 1 + (imageInTimeLine.scaleRatio - 1) * ratio;
                    int width = img.width() * curScaledRatio;
                    int height = img.height() * curScaledRatio;
                    img = imageInTimeLine.image.scaled(width, height);
                }
                painter.drawImage(rect.x(), rect.y(), img);
            }
        }
        painter.setOpacity(1);
        for (const auto& textInTimeLine : ui->timeLine->textsInTimeLine)
        {
            if (framesTillNow >= textInTimeLine.startFrame && framesTillNow <= textInTimeLine.endFrame)
            {
                // TODO
                painter.setFont(textInTimeLine.font);
                painter.setPen(QPen(textInTimeLine.color));
                painter.drawText(QRect(0, 320, 640, 40), Qt::AlignCenter, textInTimeLine.text);
            }
        }
        videoImg.save(tempDir + "\\" + QString::number(framesTillNow+1) + ".jpg");

        // 调整进度条
        bar->setValue((double)framesTillNow / totalFrame * 100);

        // 更新当前帧
        if (curFrame >= ui->timeLine->videosInTimeLine[curRank].images.size()-1)
        {
            curFrame = 0;
            curRank++;
        }
        else
        {
            curFrame++;
        }
    }
    // TODO: 导出图片的位置不对

    ffmpeg::output(tempDir, outputPath, Utils::fps);

    bar->setValue(100);
    bar->close();
}

void MainWindow::showFadeDialog()
{
    FadeEffectDialog* dialog = new FadeEffectDialog(this);
    dialog->show();

    connect(dialog, SIGNAL(sendFadeMessage(int, int)), ui->timeLine, SLOT(tryFade(int, int)));
}

void MainWindow::showMoveDialog()
{
    MoveEffectDialog* dialog = new MoveEffectDialog(this);
    dialog->show();

    connect(dialog, SIGNAL(sendMoveMessage(int)), ui->timeLine, SLOT(tryMove(int)));
}

void MainWindow::showSizeDialog()
{
    SizeEffectDialog* dialog = new SizeEffectDialog(this);
    dialog->show();

    connect(dialog, SIGNAL(sendSizeMessage(double)), ui->timeLine, SLOT(tryScale(double)));
}
