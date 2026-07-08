/**
 * @file game_view.h
 * @brief 游戏视图 —— QGraphicsView 子类，60FPS 游戏循环 + 键盘输入
 */

#pragma once

#include <QGraphicsView>
#include <QTimer>
#include <QElapsedTimer>
#include <QSet>
#include <memory>

class GameEngine;
class GameScene;
class HUD;

class GameView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GameView(QWidget* parent = nullptr);
    ~GameView() override;

    void startSingleGame(int levelId = 1);
    void startMultiGame();

    void setPaused(bool paused);
    bool isPaused() const { return m_paused; }

    GameEngine* engine() const { return m_engine.get(); }
    HUD* hud() const { return m_hud; }

signals:
    void gameFinished(int winner);
    void backToMenuRequested();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void gameLoop();
    void onGameOver(int winner);

private:
    void setupView();
    void connectSignals();

    std::unique_ptr<GameEngine> m_engine;
    GameScene* m_scene = nullptr;
    HUD* m_hud = nullptr;
    QTimer* m_timer = nullptr;
    QElapsedTimer m_frameTimer;

    bool m_paused = false;
    QSet<int> m_pressedKeys;
};
