/**
 * @file hud.h
 * @brief HUD（平视显示器）—— 生命值、分数、敌人数量、技能冷却
 *
 * v2.0 新增：技能冷却指示器、冰冻/隐身状态指示
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class GameEngine;

class HUD : public QWidget {
    Q_OBJECT
public:
    explicit HUD(QWidget* parent = nullptr);
    ~HUD() override = default;

    void attachToEngine(GameEngine* engine);
    void refresh();

private:
    GameEngine* m_engine = nullptr;

    // 玩家1
    QLabel* m_player1Label = nullptr;
    QLabel* m_player1Lives = nullptr;
    QLabel* m_player1Score = nullptr;
    QLabel* m_player1Skill = nullptr;

    // 玩家2
    QLabel* m_player2Label = nullptr;
    QLabel* m_player2Lives = nullptr;
    QLabel* m_player2Score = nullptr;
    QLabel* m_player2Skill = nullptr;

    // 游戏信息
    QLabel* m_enemyCount = nullptr;
    QLabel* m_gameStatus = nullptr;
    QLabel* m_levelInfo = nullptr;
};
