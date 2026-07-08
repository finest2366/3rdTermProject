/**
 * @file patrol_ai.h
 * @brief 巡逻 AI —— 使用决策树的巡逻型 AI
 *
 * 决策树优先级：
 *   1. 玩家在火线内 → 开火
 *   2. 可见道具 → 移向最近道具
 *   3. 否则 → 按路径巡逻
 */

#pragma once

#include "ai_controller.h"
#include <memory>

class DecisionNode;

class PatrolAI : public AIController {
public:
    PatrolAI();
    ~PatrolAI() override = default;

    void update(EnemyTank* tank, const GameState& state) override;
    QString name() const override { return QStringLiteral("PatrolAI"); }

private:
    std::unique_ptr<DecisionNode> m_root;  // 决策树根节点
};
