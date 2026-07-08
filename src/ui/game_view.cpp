/**
 * @file game_view.cpp
 * @brief 游戏视图实现 —— 主游戏循环、QPainter 渲染、输入处理
 */

#include "game_view.h"
#include "game_scene.h"
#include "hud.h"
#include "../core/game_engine.h"
#include "../core/game_config.h"
#include "../core/player_tank.h"

#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QVBoxLayout>

GameView::GameView(QWidget* parent)
    : QWidget(parent)
{
    setupView();

    m_engine = std::make_unique<GameEngine>(this);

    // HUD 叠加层
    m_hud = new HUD(this);

    connectSignals();

    // 60 FPS 定时器
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameView::gameLoop);
    m_lastFrameTimer.start();
}

GameView::~GameView() {
    m_timer->stop();
}

void GameView::setupView() {
    m_scene = new GameScene(this);

    int sceneW = GameConfig::MAP_COLS * GameConfig::CELL_SIZE;
    int sceneH = GameConfig::MAP_ROWS * GameConfig::CELL_SIZE;

    setFixedSize(sceneW + 4, sceneH + 4);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(20, 20, 30));  // 深蓝灰背景
    setPalette(pal);
}

void GameView::connectSignals() {
    connect(m_engine.get(), &GameEngine::gameOver, this, &GameView::onGameOver);
}

void GameView::startSingleGame(int levelId) {
    m_engine->startSingleGame(levelId);
    m_paused = false;
    m_timer->start(GameConfig::FRAME_MS);
    m_scene->setEngine(m_engine.get());
    m_hud->refresh();
    m_lastFrameTimer.restart();
    setFocus();
    qDebug() << "GameView: single game started, level" << levelId;
}

void GameView::startMultiGame() {
    m_engine->startMultiGame();
    m_paused = false;
    m_timer->start(GameConfig::FRAME_MS);
    m_scene->setEngine(m_engine.get());
    m_hud->refresh();
    m_lastFrameTimer.restart();
    setFocus();
    qDebug() << "GameView: multi game started";
}

void GameView::setPaused(bool paused) {
    m_paused = paused;
    if (paused) {
        m_timer->stop();
    } else {
        m_timer->start(GameConfig::FRAME_MS);
        m_lastFrameTimer.restart();
    }
}

void GameView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    // 像素游戏不需要抗锯齿，关闭可提升性能
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (m_engine) {
        m_scene->render(painter, m_engine.get());
    }
}

void GameView::gameLoop() {
    if (m_paused) return;

    // 使用实际帧时间（而非硬编码 16ms）
    qint64 elapsed = m_lastFrameTimer.restart();
    if (elapsed > 200) elapsed = 16;  // 防止死亡螺旋（超过200ms按正常帧处理）
    int frameDelta = static_cast<int>(elapsed);

    // 处理持续按下的按键
    for (int playerIdx = 0; playerIdx < 2; ++playerIdx) {
        int direction = -1;
        bool fire = false;

        if (playerIdx == 0) {
            // 玩家1: WASD + Space
            if (m_pressedKeys.contains(Qt::Key_W)) direction = 0;
            else if (m_pressedKeys.contains(Qt::Key_S)) direction = 2;
            else if (m_pressedKeys.contains(Qt::Key_A)) direction = 3;
            else if (m_pressedKeys.contains(Qt::Key_D)) direction = 1;
            fire = m_pressedKeys.contains(Qt::Key_Space);
        } else if (m_engine->isMultiPlayer()) {
            // 玩家2: 方向键 + Enter
            if (m_pressedKeys.contains(Qt::Key_Up)) direction = 0;
            else if (m_pressedKeys.contains(Qt::Key_Down)) direction = 2;
            else if (m_pressedKeys.contains(Qt::Key_Left)) direction = 3;
            else if (m_pressedKeys.contains(Qt::Key_Right)) direction = 1;
            fire = m_pressedKeys.contains(Qt::Key_Return);
        }

        if (direction >= 0 || fire) {
            m_engine->handleInput(playerIdx, direction, fire);
        }
    }

    m_engine->update(frameDelta);

    // QPainter 渲染成本极低，每帧直接重绘
    update();

    m_hud->refresh();
}

void GameView::keyPressEvent(QKeyEvent* event) {
    m_pressedKeys.insert(event->key());

    // ESC 暂停
    if (event->key() == Qt::Key_Escape) {
        if (!m_engine->isGameOver()) {
            setPaused(!m_paused);
        }
        return;
    }

    // 技能键（玩家1: Q, 玩家2: Shift）
    if (event->key() == Qt::Key_Q) {
        m_engine->activateSkill(0);
    }
    if (event->key() == Qt::Key_Shift && m_engine->isMultiPlayer()) {
        m_engine->activateSkill(1);
    }

    QWidget::keyPressEvent(event);
}

void GameView::keyReleaseEvent(QKeyEvent* event) {
    m_pressedKeys.remove(event->key());
    QWidget::keyReleaseEvent(event);
}

void GameView::onGameOver(int winner) {
    m_timer->stop();
    qDebug() << "Game over! Winner:" << winner;
    emit gameFinished(winner);
}
