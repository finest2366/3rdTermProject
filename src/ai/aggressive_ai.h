/**
 * @file aggressive_ai.h
 * @brief 激进 AI —— 使用决策树主动追击玩家
 *
 * 决策树优先级：
 *   1. 玩家在火线内 → 开火
 *   2. 低血量 → 逃跑
 *   3. 可见道具 → 拾取
 *   4. 否则 → 追击玩家
 *
 * 支持 Boss AI 模式：连发射击 + 血量低于阈值时逃跑
 */

#pragma once

#include "ai_controller.h"
#include <memory>

class DecisionNode;

class AggressiveAI : public AIController {
public:
    AggressiveAI();
    ~AggressiveAI() override = default;

    void update(EnemyTank* tank, const GameState& state) override;
    QString name() const override { return QStringLiteral("AggressiveAI"); }

private:
    std::unique_ptr<DecisionNode> m_root;
    int m_burstCounter = 0;  // Boss 连发计数
};
