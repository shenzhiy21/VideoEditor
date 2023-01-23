#include "videocutdialog.h"
#include "ui_videocutdialog.h"
#include "ffmpeg.h"
#include "utils.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QFile>

VideoCutDialog::VideoCutDialog(const QString& path, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::VideoCutDialog),
    path(path)
{
    ui->setupUi(this);
    this->setWindowTitle("裁剪");
    qDebug() << path;
    showVideo();
    playbackState = QMediaPlayer::PlayingState;
    sliderState = false;

    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(getDuration(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(getPosition(qint64)));
    connect(ui->pauseBtn, SIGNAL(clicked()), this, SLOT(playerPause()));
    connect(ui->playBtn, SIGNAL(clicked()), this, SLOT(playerPlay()));
    connect(ui->breakBtn_1, SIGNAL(clicked()), this, SLOT(getStartTime()));
    connect(ui->breakBtn_2, SIGNAL(clicked()), this, SLOT(getEndTime()));
    connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(checkOut()));
    connect(ui->slider, &MySlider::mousePress, [&](int val){
        sliderState = true;
        playbackState = QMediaPlayer::PausedState;
        player->pause();
        qDebug() << "鼠标按下 " << val;
    });
    connect(ui->slider, &MySlider::mouseRelease, [&](int val){
        sliderState = false;
        playbackState = QMediaPlayer::PlayingState;
        player->setPosition(val);
        ui->slider->setValue(val);
        player->play();
        qDebug() << "鼠标松开 " << val;
    });

    Utils::mySetIcon(ui->playBtn, ":/res/play.svg");
    Utils::mySetIcon(ui->pauseBtn, ":/res/pause.svg");
    Utils::mySetIcon(ui->breakBtn_1, ":/res/scissors.svg");
    Utils::mySetIcon(ui->breakBtn_2, ":/res/scissors.svg");
    Utils::mySetIcon(ui->checkBtn, ":/res/check.svg");
}

VideoCutDialog::~VideoCutDialog()
{
    delete ui;
}

void VideoCutDialog::showVideo()
{
    player = new QMediaPlayer(this);
    player->setSource(QUrl::fromLocalFile(path));
    QAudioOutput* audioOutput = new QAudioOutput(this);
    player->setVideoOutput(ui->videoWidget);
    player->setAudioOutput(audioOutput);
    player->play();
}

void VideoCutDialog::getDuration(qint64 playtime)
{
    if (!sliderState)
    {
        playtime = player->duration();
        setDuration(playtime);
    }
}

void VideoCutDialog::setDuration(qint64 playtime)
{
    ui->slider->setMinimum(0);
    ui->slider->setMaximum(playtime);
}

void VideoCutDialog::getPosition(qint64 playtime)
{
    if (!sliderState)
    {
        playtime = player->position();
        setPosition(playtime);
    }
}

void VideoCutDialog::setPosition(qint64 playtime)
{
    if (!sliderState)
    {
        ui->slider->setValue(playtime);
    }
}

void VideoCutDialog::playerPause()
{
    this->player->pause();
    playbackState = QMediaPlayer::PausedState;
}

void VideoCutDialog::playerPlay()
{
    this->player->play();
    playbackState = QMediaPlayer::PlayingState;
}

void VideoCutDialog::keyPressEvent(QKeyEvent* event)
{
    // "Space" 快捷键：可以控制播放器播放与暂停的状态
    if (event->key() == Qt::Key_Space)
    {
        if (playbackState == QMediaPlayer::PlayingState)
        {
            playbackState = QMediaPlayer::PausedState;
            player->pause();
        }
        else if (playbackState == QMediaPlayer::PausedState)
        {
            playbackState = QMediaPlayer::PlayingState;
            player->play();
        }
    }
}

void VideoCutDialog::getStartTime()
{
    player->pause();
    playbackState = QMediaPlayer::PausedState;
    startTime = player->position();
}

void VideoCutDialog::getEndTime()
{
    player->pause();
    playbackState = QMediaPlayer::PausedState;
    endTime = player->position();
}

void VideoCutDialog::checkOut()
{
    // 确认起始时间点和终止时间点是合法的
    if ((endTime < 0) || (startTime < 0) || (startTime >= endTime)) return;

    // -----开始裁剪-----
    // 获得导出路径
    QFileDialog dialog;
    QString outputPath = dialog.getSaveFileName(this, tr("Open File"), QString(), tr("Videos (*.mp4 *.mkv *.avi)"));

    // 获取两个时间的时间戳
    qint64 duration = endTime - startTime;
    QString cutStartTime = Utils::calculateTime(startTime);
    QString durationTime = Utils::calculateTime(duration);

    // 调用 ffmpeg 进行裁剪
    ffmpeg::cutVideo(path, outputPath, cutStartTime, durationTime);

    // 发送新增文件信号
    emit cutVideoMessage(outputPath);
}
