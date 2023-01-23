QT       += core gui multimediawidgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fadeeffectdialog.cpp \
    ffmpeg.cpp \
    imagetimeline.cpp \
    imageview.cpp \
    main.cpp \
    mainwindow.cpp \
    moveeffectdialog.cpp \
    mypushbutton.cpp \
    myslider.cpp \
    previewwindow.cpp \
    sizeeffectdialog.cpp \
    subtitleprocesser.cpp \
    texteditdialog.cpp \
    texteditor.cpp \
    texttimeline.cpp \
    timeline.cpp \
    utils.cpp \
    videocutdialog.cpp \
    videotimeline.cpp \
    videoview.cpp

HEADERS += \
    fadeeffectdialog.h \
    ffmpeg.h \
    imagetimeline.h \
    imageview.h \
    mainwindow.h \
    moveeffectdialog.h \
    mypushbutton.h \
    myslider.h \
    previewwindow.h \
    sizeeffectdialog.h \
    subtitleprocesser.h \
    texteditdialog.h \
    texteditor.h \
    texttimeline.h \
    timeline.h \
    utils.h \
    videocutdialog.h \
    videotimeline.h \
    videoview.h

FORMS += \
    fadeeffectdialog.ui \
    mainwindow.ui \
    moveeffectdialog.ui \
    sizeeffectdialog.ui \
    texteditdialog.ui \
    texteditor.ui \
    videocutdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
