#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_NikaidoHiro.h"
#include<qtimer.h>
#include<qlabel.h>
#include<qpainter.h>
#include<qmovie.h>
#include<QMouseEvent>
#include<qmenu.h>
#include <QDesktopServices>
#include <QUrl>
#include<qrandom.h>
#include <qsoundeffect.h>
#include<qmap.h>
#include<qfile.h>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
const double SMALL = 0.5;
const double MEDIAN = 1;
const double BIG =1.5;



class NikaidoHiro : public QMainWindow
{
    Q_OBJECT
public:
    NikaidoHiro(QWidget* parent = nullptr);
    ~NikaidoHiro();
    enum State
    {
        Normal,
        Think,
        Panic,
        Pride,
        Demonize,
        Special,
        Sleep,
        Yawn,
        Head,
        _Head,
    };
protected:
    //重写基类的虚函数
    //void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    //计时器函数
    void creat_clickTimer();
    void creat_sleepTimer();
    //计时函数
    void resetTime();
    //单击双击事件函数
    void LeftSingleClickEvent();
    void doubleClickEvent();
    //GIF函数
    void normalMovie();
    void demonizeMovie();
    void changeMovie(QString filename);
    //音频函数
    void creatMusic();
    void creatVoiceMap();
    void changeMusic(QString filename);
    void changeVoice(QString filename);
    //创建菜单函数
    void creatMenu();
    //调整大小函数
    void changeSize();

private:
    Ui::NikaidoHiroClass ui;
    QMap<QString, QString> voiceMap;
    QLabel* Hiro_label;
    QMovie* Hiro_movie;
    QPoint m_dragPosition;
    QPoint m_startPosition;
    QPoint M_startPosition;
    QPoint framePosition;
    QPoint location;
    QMediaPlayer* bgmPlayer;
    QAudioOutput* bgmOutput;
    QSoundEffect* music;
    QSoundEffect* voice;
    QMenu* menu;
    QMenu* submenu;
    QTimer* clickTimer;//判断鼠标动作的计时器
    QTimer* sleepTimer;//用于计算希罗什么时候睡觉
    bool is_doubleclick = false;
    bool isdragging = false;
    bool isdemonize = false;
    bool ismute = false;
    double scale=MEDIAN;
    double x, y;
    int idletime = 0;
    //菜单栏选项
    QAction* quit;
    QAction* think;
    QAction* openBilbil;
    QAction* demonize;
    QAction* sleep;
    QAction* sub;
    QAction* small;
    QAction* median;
    QAction* big;
    QAction* mute;
    State state;
};


