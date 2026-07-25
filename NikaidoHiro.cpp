#include "NikaidoHiro.h"
//构造函数定义
NikaidoHiro::NikaidoHiro(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //1.去掉标题栏
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setStyleSheet("background: transparent; border: none;");
    //2.设置背景透明
    setAttribute(Qt::WA_TranslucentBackground);
    //3.设置窗口大小
    setFixedSize(150, 250);
    //初始化音频
    creatMusic();
    creatVoiceMap();
    //加载希罗的待机GIF
    Hiro_movie = new QMovie(":/GIF/stand/1.gif", QByteArray(), this);
    Hiro_label = new QLabel(this);
    Hiro_label->setFixedSize(this->size());//2
    Hiro_label->setStyleSheet("background:transparent;border:none;");//1
    Hiro_movie->setScaledSize(Hiro_label->size());//3
    Hiro_label->setMovie(Hiro_movie);
    normalMovie();
    //nowScreen = QGuiApplication::primaryScreen();
    //QRect desktopRect = nowScreen->availableGeometry();
    //初始化菜单
    creatMenu();
    //创建单双击计时器
    creat_clickTimer();
    creat_sleepTimer();
    //打开睡觉计时器
    sleepTimer->start();
    //建立连接
    connect(Hiro_movie, &QMovie::frameChanged, [=](int frame)
        {
            if (state == Sleep)
            {
                //关闭睡觉计时器并清零累计空闲时间
                idletime = 0;
                sleepTimer->stop();
                //睡觉状态播放到最后一帧时停止
                if (frame == Hiro_movie->frameCount() - 1)
                    Hiro_movie->stop();
            }
            //除了正常状态、魔女化状态和睡觉状态，其他的根据状态决定播放原GIF
            if (state != Normal && state!=Demonize &&state!=Sleep&& frame == Hiro_movie->frameCount() - 1)
            {
                if (!isdemonize)
                {
                    normalMovie();
                }
                else
                    demonizeMovie();
            }
        });
}
//析构函数定义
NikaidoHiro::~NikaidoHiro()
{
}
//播放待机GIF
void NikaidoHiro::normalMovie()
{
    //播放背景音乐
    changeMusic(":/Sound/music/1.wav");
    //如果是从魔女化状态变回正常状态，就重新打开睡觉计时器
    if (state == Demonize||state == Sleep )
    {
        sleepTimer->start();
        idletime = 0;
    }
    isdemonize = false;
    state = Normal;
    if (Hiro_movie->fileName() != ":/GIF/stand/1.gif")
        changeMovie(":/GIF/stand/1.gif");
    Hiro_movie->start();
}
void NikaidoHiro::demonizeMovie()
{
    //播放背景音乐
    changeMusic(":/Sound/music/2.wav");
    isdemonize = true;
    state = Demonize;
    //魔女化时关闭睡觉计时器并清零累计空闲时间
    if (sleepTimer->isActive())
    {
        sleepTimer->stop();
        idletime = 0;
    }
    if (Hiro_movie->fileName() != ":/GIF/_stand/1.gif")
        changeMovie(":/GIF/_stand/1.gif");
}
//鼠标点击事件
void NikaidoHiro::mousePressEvent(QMouseEvent* event)
{
    // 鼠标左键按下时
    if (event->button() == Qt::LeftButton) {
        // 记录鼠标相对于窗口左上角的偏移量，并记录正在拖动
        qDebug() << "坐标：" << event->position().toPoint();
        isdragging = true;
        m_startPosition = event->globalPosition().toPoint();
        framePosition= frameGeometry().topLeft();
        m_dragPosition = m_startPosition - framePosition;
        if (clickTimer->isActive())
        {
            //条件满足时，为双击
            is_doubleclick = true;
            qDebug() << "双击";
            clickTimer->stop();
        }
        // 标记该事件已被处理
        event->accept();
    }
    QWidget::mousePressEvent(event);
}
//鼠标释放事件
void  NikaidoHiro::mouseReleaseEvent(QMouseEvent* event)
{
    isdragging = false;
    if(m_startPosition==event->globalPosition().toPoint() && event->button() == Qt::LeftButton)
    {
        if (is_doubleclick)
        {
            doubleClickEvent();
            is_doubleclick = false;
        }
        else
            clickTimer->start();
    }
    QWidget::mouseReleaseEvent(event);
}
//鼠标移动事件
void NikaidoHiro::mouseMoveEvent(QMouseEvent* event)
{
    //必须是左键按下且不松开
    if (event->buttons() & Qt::LeftButton)//处理逻辑：按位与判断是否含有鼠标左键
    {
        //使用move函数将窗口移动至鼠标位置
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}
//左键单击事件
void NikaidoHiro::LeftSingleClickEvent()
{
    if (state == Sleep)
        return;
    //左键单击
    location = this->mapFromGlobal(QCursor::pos());
    x = location.x();
    y = location.y();
    double random = QRandomGenerator::global()->generateDouble();
    qDebug() << "y=" << y;
    //点击头部
    if (y >= 0.0 && y <= 50.0 * scale)
    {
        if(state==Normal)
        {
            state = Head;
            if (random < 0.3)
                changeMovie(":/GIF/head/1.gif");
            else if (random >= 0.3 && random < 0.6)
                changeMovie(":/GIF/head/2.gif");
            else
                changeMovie(":/GIF/head/3.gif");
        }
        if (state == Demonize)
        {
            state = _Head;
            if (random < 0.3)
                changeMovie(":/GIF/_head/1.gif");
            else if (random >= 0.3 && random < 0.6)
                changeMovie(":/GIF/_head/2.gif");
            else 
                changeMovie(":/GIF/_head/3.gif");
        }
    }
}
//右键单击事件
void NikaidoHiro::contextMenuEvent(QContextMenuEvent* event)
{
    //睡觉时禁用调整大小功能
    submenu->setEnabled(state != Sleep);
    //魔女化时禁用睡觉按钮
    sleep->setEnabled(state != Demonize);
    menu->exec(QCursor::pos());
    event->accept();
}
//双击事件
void NikaidoHiro::doubleClickEvent()
{
    if(state==Normal)
    {
        state = Special;
        /*double random2 = QRandomGenerator::global()->generateDouble();*/
        changeMovie(":/GIF/special/2.gif");

    }
    if (state == Demonize)
    {
        state = Special;
        changeMovie(":/GIF/special/1.gif");
    }
    //处于睡觉状态则双击叫醒希罗
    if(state==Sleep)
        normalMovie();
}
//初始化菜单
void NikaidoHiro::creatMenu()
{
    menu = new QMenu(this);
    menu->setStyleSheet(R"(
        QMenu {
    background-color: #FFF2F5;           
    border: 2px solid #A61C2E;          
    border-radius: 10px;               
    padding: 6px 4px;
}
QMenu::item {
    color: #232323;                      
    font-family: 楷体;
    font-size: 14px;
    font-weight: 200;
    padding: 4px 16px 4px 12px;        
    background-color: transparent;
}
/* 禁用状态样式 */
QMenu::item:disabled {
        color: #CCCCCC;                
        background-color: transparent;
} 
/* 禁用的子菜单标题 */
QMenu::item:disabled:selected {
        background-color: transparent;
        color: #CCCCCC;
}
QMenu::item:selected {
    background-color: #A61C2E;           
    color: #FFFFFF;                     
    border-radius: 3px;                  
}
QMenu::separator {
    height: 1px;
    background-color: #FFC0CB;          
    margin: 3px 6px;
}
    )");
    submenu = menu->addMenu("大小");
    small = submenu->addAction("小");
    median = submenu->addAction("中");
    big = submenu->addAction("大");
    demonize = menu->addAction("魔女化");
    demonize->setCheckable(true);
    demonize->setChecked(false);
    sleep = menu->addAction("睡觉");
    openBilbil = menu->addAction("打开B站");
    mute = menu->addAction("静音");
    mute->setCheckable(true);
    mute->setChecked(false);
    quit=menu->addAction("退出");
    //实现按钮功能
    connect(small, &QAction::triggered, [=]()
        {
            scale = SMALL;
            //关掉GIF播放，这里是因为可以使调整大小时更加自然，不出现类似掉帧的情况
            Hiro_movie->stop();
            changeSize();
        });
    connect(median, &QAction::triggered, [=]()
        {
            scale = MEDIAN;
            Hiro_movie->stop();
            changeSize();
        });
    connect(big, &QAction::triggered, [=]()
        {
            scale = BIG;
            Hiro_movie->stop();
            changeSize();
        });
    connect(quit, &QAction::triggered, [=]() {
        QApplication::quit();
        });
    connect(openBilbil, &QAction::triggered, [=]()
        {
            QDesktopServices::openUrl(QUrl("https://www.bilibili.com/video/BV1GJ411x7h7/?spm_id_from=333.337.search-card.all.click&vd_source=69be0109070081db0f9e3516eca59c80"));
        });
    connect(demonize, &QAction::triggered, [=](bool checked)
        {
            if (checked)
            {
                demonizeMovie();
            }
            else
            {
                normalMovie();
            }
        });
    connect(mute, &QAction::triggered, [=](bool checked)
        {
            if (checked)
            {
                bgmPlayer->stop();
                voice->stop();
                ismute = true;
            }
            else
            {
                bgmPlayer->play();
                ismute = false;
            }
        });
    connect(sleep, &QAction::triggered, [=]()
        {
            //关闭音乐
            bgmPlayer->stop();
            if(state!=Sleep)
            {
                state = Sleep;
                changeMovie(":/GIF/sleep/1.gif");
            }
        });
}
//调整大小
void NikaidoHiro::changeSize()
{
    setFixedSize(150*scale, 250*scale);
    Hiro_label->setFixedSize(this->size());
    Hiro_movie->setScaledSize(Hiro_label->size());
    Hiro_label->setMovie(Hiro_movie);
    Hiro_movie->start();
}
//变换GIF函数
void NikaidoHiro::changeMovie(QString filename)
{
    Hiro_movie->stop();
    Hiro_movie->setFileName(filename);
    Hiro_movie->start();
    qDebug() << "state=" << state << "\n" << voiceMap[filename];
    if (state != Normal && state != Sleep && state != Demonize)
    {
        changeVoice(voiceMap[filename]);
    }
}
//音频函数
void NikaidoHiro::creatMusic()
{
    bgmPlayer = new QMediaPlayer(this);
    bgmOutput = new QAudioOutput(this);
    bgmPlayer->setAudioOutput(bgmOutput);
    bgmOutput->setVolume(0.3);
    // 循环播放
    bgmPlayer->setLoops(QMediaPlayer::Infinite);
    voice = new QSoundEffect(this);
    voice->setVolume(1.2);
}
void NikaidoHiro::changeMusic(QString filename)
{
    QString fullPath;
    if (filename.startsWith(":/"))
    {
        fullPath = "qrc" + filename;
    }
    else
    {
        fullPath = QUrl::fromLocalFile(filename).toString();
    }
    if (bgmPlayer->source().toString() == fullPath)
    {
        //如果已经静音，直接返回
        if (ismute)
            return;
        if (bgmPlayer->playbackState() != QMediaPlayer::PlayingState)
            bgmPlayer->play();
        return;
    }
    bgmPlayer->stop();
    bgmPlayer->setSource(QUrl(fullPath));
    //如果已经静音，直接返回
    if (ismute)
        return;
    bgmPlayer->play();
    qDebug() << "路径:" << fullPath;
    qDebug() << "播放状态:" << bgmPlayer->playbackState();
}
void NikaidoHiro::changeVoice(QString filename)
{
    if (ismute)
        return;
    QString fullPath;
    if (filename.startsWith(":/"))
    {
        fullPath = "qrc" + filename;
    }
    else
    {
        fullPath = QUrl::fromLocalFile(filename).toString();
    }
    voice->stop();
    voice->setSource(QUrl(fullPath));
    voice->play();
}
void NikaidoHiro::creatVoiceMap()
{
    voiceMap[":/GIF/head/1.gif"] = ":/Sound/voice/wait me.wav";
    voiceMap[":/GIF/head/2.gif"] = ":/Sound/voice/plan.wav";
    voiceMap[":/GIF/head/3.gif"] = ":/Sound/voice/Ema.wav";
    voiceMap[":/GIF/_head/1.gif"] = ":/Sound/voice/no.wav";
    voiceMap[":/GIF/_head/2.gif"] = ":/Sound/voice/that's false.wav";
    voiceMap[":/GIF/_head/3.gif"] = ":/Sound/voice/you're not correct.wav";
    voiceMap[":/GIF/special/1.gif"] = ":/Sound/voice/I'll control all of you.wav";
    voiceMap[":/GIF/special/2.gif"] = ":/Sound/voice/Are you listening.wav";
    voiceMap[":/GIF/yawn/1.gif"] = ":/Sound/voice/yawn.wav";
}
//计时器函数
void NikaidoHiro::creat_clickTimer()
{
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    clickTimer->setInterval(500);
    connect(clickTimer, &QTimer::timeout, [=]()
        {
            //if(m_startPosition == framePosition)//防止拖动时意外触发点击事件
            //{
                LeftSingleClickEvent();
                qDebug() << "yes!";
            //}
        });
}
void NikaidoHiro::creat_sleepTimer()
{
    sleepTimer = new QTimer(this);
    sleepTimer->setSingleShot(true);
    sleepTimer->setInterval(60000);
    connect(sleepTimer, &QTimer::timeout, [=]()
        {
            sleepTimer->start();
            idletime++;
            resetTime();
        });
}
//计时函数
void NikaidoHiro::resetTime()
{
    if (idletime == 20)
    {
        state = Sleep;
        //关闭音乐
        bgmPlayer->stop();
        //清空计时器
        idletime = 0;
        changeMovie(":/GIF/sleep/1.gif");
    }
    if (idletime > 0 && idletime % 5 == 0&&idletime!=20)
    {
        state = Yawn;
        changeMovie(":/GIF/yawn/1.gif"); 
    }
}
//每个特殊changemovie函数之后都要添加changevoice和更新state