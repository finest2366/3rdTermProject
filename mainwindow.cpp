#include "mainwindow.h"
#include "gamescene.h"
#include "startscene.h"
#include "gamemanager.h"

#include <QGraphicsView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("坦克大战 - Tank Battle");

    m_manager = new GameManager(this);

    createUI();
    createMenus();

    // 窗口大小
    setMinimumSize(SCENE_WIDTH + 20, SCENE_HEIGHT + menuBar()->height() + statusBar()->height() + 20);
    resize(SCENE_WIDTH + 20, SCENE_HEIGHT + menuBar()->height() + statusBar()->height() + 40);

    // 连接信号
    connect(m_manager, &GameManager::stateChanged, this, [this](GameManager::State) {
        updateStatusBar();
    });
    connect(m_manager, &GameManager::scoreChanged, this, [this](int, int) {
        updateStatusBar();
    });

    updateStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createUI()
{
    // 创建图形视图
    m_view = new QGraphicsView(this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setBackgroundBrush(GameColors::BACKGROUND);
    m_view->setFocusPolicy(Qt::StrongFocus);

    // 安装事件过滤器捕获键盘事件
    m_view->installEventFilter(this);

    // 创建游戏场景（稍后用到）
    m_scene = new GameScene(m_manager, this);
    connect(m_scene, &GameScene::gameOver, this, &MainWindow::onGameOver);

    // 创建开始界面
    m_startScene = new StartScene(this);
    connect(m_startScene, &StartScene::singlePlayerSelected, this, &MainWindow::onNewSinglePlayer);
    connect(m_startScene, &StartScene::twoPlayerSelected, this, &MainWindow::onNewTwoPlayer);

    // 初始显示开始界面
    m_view->setScene(m_startScene);
    m_view->setFocus();

    setCentralWidget(m_view);

    // 状态栏
    m_statusLabel = new QLabel("准备开始");
    m_scoreLabel = new QLabel();
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_scoreLabel);
}

void MainWindow::createMenus()
{
    // 游戏菜单
    QMenu* gameMenu = menuBar()->addMenu("游戏(&G)");

    QAction* singleAction = gameMenu->addAction("单人闯关(&S)");
    singleAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(singleAction, &QAction::triggered, this, &MainWindow::onNewSinglePlayer);

    QAction* pvpAction = gameMenu->addAction("双人对战(&T)");
    pvpAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(pvpAction, &QAction::triggered, this, &MainWindow::onNewTwoPlayer);

    gameMenu->addSeparator();

    QAction* pauseAction = gameMenu->addAction("暂停/继续(&P)");
    pauseAction->setShortcut(QKeySequence("P"));
    connect(pauseAction, &QAction::triggered, this, &MainWindow::onPause);

    gameMenu->addSeparator();

    QAction* quitAction = gameMenu->addAction("退出(&Q)");
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuit);

    // 帮助菜单
    QMenu* helpMenu = menuBar()->addMenu("帮助(&H)");

    QAction* aboutAction = helpMenu->addAction("操作说明(&C)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::updateStatusBar()
{
    switch (m_manager->state()) {
    case GameManager::WAITING:
        m_statusLabel->setText("准备开始 - 请选择游戏模式");
        m_scoreLabel->clear();
        break;
    case GameManager::PLAYING: {
        if (m_scene && m_scene->player1()) {
            int p1 = m_manager->player1Score();
            int p2 = m_manager->player2Score();
            QString mode = (m_scene->player2() ? "双人对战" : "单人闯关");
            m_statusLabel->setText(QString("进行中 [%1]  玩家1: %2 分").arg(mode).arg(p1));
            if (m_scene->player2())
                m_scoreLabel->setText(QString("玩家2: %1 分 | 先得 %2 分获胜").arg(p2).arg(WIN_SCORE_PVP));
            else
                m_scoreLabel->setText(QString("关卡: %1 | 剩余敌人: %2")
                    .arg(m_manager->currentLevel() + 1)
                    .arg(m_manager->enemiesRemaining()));
        }
        break;
    }
    case GameManager::PAUSED:
        m_statusLabel->setText("已暂停 - 按 P 继续");
        break;
    case GameManager::WON:
        m_statusLabel->setText("恭喜！你赢了！");
        break;
    case GameManager::LOST:
        m_statusLabel->setText("游戏失败！基地被摧毁了！");
        break;
    }
}

// ===== 事件处理 =====

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_view) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            m_scene->keyPressed(keyEvent->key());
            return false; // 继续传递事件
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            m_scene->keyReleased(keyEvent->key());
            return false;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

// ===== 菜单槽函数 =====

void MainWindow::onNewSinglePlayer()
{
    m_view->setScene(m_scene);
    m_manager->startSinglePlayer();  // 重置关卡和分数
    m_scene->startSinglePlayer(0);
    m_view->setFocus();
}

void MainWindow::onNewTwoPlayer()
{
    m_view->setScene(m_scene);
    m_manager->startPvP();  // 重置分数
    m_scene->startPvP();
    m_view->setFocus();
}

void MainWindow::onPause()
{
    if (m_scene->isRunning()) {
        m_scene->pause();
    } else {
        m_scene->resume();
    }
}

void MainWindow::onQuit()
{
    close();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "操作说明",
        "<h2>坦克大战 - 操作说明</h2>"
        "<h3>单人闯关模式</h3>"
        "<p><b>玩家1:</b></p>"
        "<ul>"
        "<li>移动: <b>W A S D</b></li>"
        "<li>射击: <b>空格键</b></li>"
        "<li>暂停: <b>P</b> 或 <b>Esc</b></li>"
        "</ul>"
        "<p>消灭所有敌方坦克即可过关，保护你的基地！</p>"
        "<p>砖墙可被摧毁，钢铁墙不可摧毁。水域不能通过但子弹可以通过。</p>"
        "<p>草丛会遮挡视野。</p>"
        "<hr>"
        "<h3>双人对战模式</h3>"
        "<p><b>玩家1:</b> 移动 <b>W A S D</b>，射击 <b>空格键</b></p>"
        "<p><b>玩家2:</b> 移动 <b>方向键 ↑↓←→</b>，射击 <b>/</b></p>"
        "<p>先击杀对方 " + QString::number(WIN_SCORE_PVP) + " 次者获胜！</p>");
}

void MainWindow::onGameOver(bool won)
{
    QString msg;
    if (won) {
        if (m_scene->player2()) {
            int p1 = m_manager->player1Score();
            int p2 = m_manager->player2Score();
            msg = QString("玩家1: %1 分\n玩家2: %2 分\n\n").arg(p1).arg(p2);
            msg += (p1 >= WIN_SCORE_PVP) ? "玩家1 获胜！" : "玩家2 获胜！";
        } else {
            msg = "恭喜通关！你成功消灭了所有敌人！";
        }
    } else {
        msg = "游戏结束！\n";
        if (m_scene->player2()) {
            int p1 = m_manager->player1Score();
            int p2 = m_manager->player2Score();
            msg += QString("玩家1: %1 分  玩家2: %2 分").arg(p1).arg(p2);
        } else {
            msg += "你的坦克被摧毁了！";
        }
    }

    QMessageBox::information(this, "游戏结束", msg);
    showStartScreen();
}

void MainWindow::showStartScreen()
{
    m_view->setScene(m_startScene);
    m_view->setFocus();
    m_statusLabel->setText("准备开始");
    m_scoreLabel->clear();
}
