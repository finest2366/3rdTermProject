/**
 * @file enemy_tank.h
 * @brief 敌方坦克 —— 由 AI 控制器驱动，支持多种敌人类型
 */

#pragma once

#include "tank.h"
#include <memory>

class AIController;

class EnemyTank : public Tank {
public:
    EnemyTank(TankType enemyType, const QPointF& pos, std::unique_ptr<AIController> ai);
    ~EnemyTank() override = default;

    Bullet* fire() override;
    void updateAI(const GameState& state) override;

    AIController* aiController() const { return m_ai.get(); }
    void setAIController(std::unique_ptr<AIController> ai) { m_ai = std::move(ai); }

private:
    std::unique_ptr<AIController> m_ai;
};
