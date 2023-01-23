#include "previewwindow.h"
#include "texttimeline.h"
#include "texteditdialog.h"
#include "texteditor.h"
#include <QPainter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPalette>
#include <QLineEdit>

PreviewWindow::PreviewWindow(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(400, 225);
    videoLabel = new QLabel(this);
    videoLabel->setGeometry(0, 0, 400, 225);
    videoLabel->setScaledContents(true);
    videoLabel->show();

    setAcceptDrops(true);
}

void PreviewWindow::showImage(int curFrame, const QImage& img, const QList<ImageTimeLine>& imageList,
                              const QList<TextTimeLine>& textList)
{
    // 加载视频（画布最底层）
    if (img.isNull()) qDebug() << "PreviewWindow: load image fail";
    videoLabel->setPixmap(QPixmap::fromImage(img));
    videoLabel->show();

    // 加载图片
    for (auto label : imageLabels) delete label;
    imageLabels.clear();
    for (const auto& imageInfo : imageList)
    {
        QLabel* label = new QLabel(this);
        label->move(imageInfo.rect.x(), imageInfo.rect.y());

        QPixmap pixmap = QPixmap::fromImage(imageInfo.image);
        if (imageInfo.isFade)
        {
            if (curFrame - imageInfo.startFrame < imageInfo.fadeInFrames)
            {
                QPixmap newPixmap(pixmap.size());
                newPixmap.fill(Qt::transparent);
                QPainter painter(&newPixmap);
                painter.setOpacity((double)(curFrame - imageInfo.startFrame) / imageInfo.fadeInFrames);
                painter.drawPixmap(0, 0, pixmap);
                label->setPixmap(newPixmap);
            }
            else if (imageInfo.endFrame - curFrame < imageInfo.fadeOutFrames)
            {
                QPixmap newPixmap(pixmap.size());
                newPixmap.fill(Qt::transparent);
                QPainter painter(&newPixmap);
                painter.setOpacity((double)(imageInfo.endFrame - curFrame) / imageInfo.fadeOutFrames);
                painter.drawPixmap(0, 0, pixmap);
                label->setPixmap(newPixmap);
            }
            else
            {
                label->setPixmap(pixmap);
            }
        }
        else
        {
            label->setPixmap(pixmap);
        }

        if (imageInfo.isMove)
        {
            int dx = (double)(imageInfo.endFrame - curFrame) * imageInfo.moveDistance / (imageInfo.endFrame - imageInfo.startFrame);
            label->move(label->pos().x() + dx, label->pos().y());
        }

        if (imageInfo.isScale)
        {
            double ratio = (double)(curFrame - imageInfo.startFrame) * imageInfo.scaleRatio / imageInfo.endFrame;
            double curScaledRatio = 1 + (imageInfo.scaleRatio - 1) * ratio;
            int width = label->pixmap().width() * curScaledRatio;
            int height = label->pixmap().height() * curScaledRatio;
            QPixmap scaledPixmap = label->pixmap().scaled(QSize(width, height));
            label->setPixmap(scaledPixmap);
        }
        label->show();
        imageLabels.append(label);
    }

    // 加载文字
    for (auto label : textLabels) delete label;
    textLabels.clear();
    for (const auto& textInfo : textList)
    {
        QLabel* label = new QLabel(this);
        label->setGeometry(textInfo.rect);
        label->setAlignment(Qt::AlignCenter);
        label->setText(textInfo.text);
        label->setFont(textInfo.font);
        QPalette pe;
        pe.setColor(QPalette::WindowText, textInfo.color);
        label->setPalette(pe);
        label->show();
        textLabels.append(label);
    }
}

void PreviewWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/image"))
    {
        event->accept();
    }
    else if (event->mimeData()->hasFormat("application/text"))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void PreviewWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/image"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat("application/text"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void PreviewWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/image"))
    {
        QByteArray data = event->mimeData()->data("application/image");
        QDataStream dataStream(&data, QIODevice::ReadOnly);

        QString path;
        QPixmap pixmap;

        dataStream >> pixmap >> path;

        QRect rect(event->position().x()-(pixmap.width()/2), event->position().y()-(pixmap.height()/2), pixmap.width(), pixmap.height());
        qDebug() << "hot spot:" << rect.x() << rect.y();

        emit imageToAdd(rect, path);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat("application/text"))
    {
        QByteArray data = event->mimeData()->data("application/text");
        QDataStream dataStream(&data, QIODevice::ReadOnly);

        QString text;
        QColor color;
        QFont font;
        QPixmap pixmap;
        dataStream >> text >> color >> font >> pixmap;
        QRect rect(event->position().x()-(pixmap.width()/2), event->position().y()-pixmap.height()/2, pixmap.width(), pixmap.height());

        TextTimeLine textInTimeLine;
        textInTimeLine.text = text;
        textInTimeLine.color = color;
        textInTimeLine.font = font;
        textInTimeLine.rect = rect;

        emit textToReplace(textInTimeLine);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void PreviewWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    int posX = event->pos().x();
    int posY = event->pos().y();

    TextEditDialog* dialog = new TextEditDialog("", true, this);
    dialog->x = posX;
    dialog->y = posY;
    dialog->show();

    connect(dialog, SIGNAL(textSignal(QString, int, int)), this, SLOT(showTextEditor(QString, int, int)));
}

void PreviewWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) return;
    auto pos = event->pos();
    for (auto label : textLabels)
    {
        if (label->geometry().contains(pos))
        {
            createTextDrag(label);
            break;
        }
    }
}

void PreviewWindow::createTextDrag(QLabel* label)
{
    // 记录 label 信息
    QColor color = label->palette().windowText().color();
    QFont font = label->font();
    QString text = label->text();

    // 创建 QDrag 所需要包含的 mime data
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << text << color << font;

    // 创建拖拽显示的图片
    QPixmap pixmap(label->size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPen pen(QColor(label->palette().windowText().color()));
    painter.setPen(pen);
    painter.setFont(label->font());
    painter.drawText(label->rect(), Qt::AlignHCenter | Qt::AlignVCenter, label->text());
    dataStream << pixmap;

    // 创建拖拽
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/text", itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
    drag->setPixmap(pixmap);
    drag->exec();
}

void PreviewWindow::showTextEditor(QString str, int x, int y)
{
    TextTimeLine text(str, 0, 0);
    text.rect = QRect(x, y, 100, 100);
    TextEditor* editor = new TextEditor(text, this);
    editor->show();
    connect(editor, SIGNAL(addText(TextTimeLine)), this, SLOT(addTextToTimeLine(TextTimeLine)));
}

void PreviewWindow::addTextToTimeLine(TextTimeLine text)
{
    emit textToLoad(text);
}
