/**
 * @file patrol_ai.cpp
 * @brief 巡逻 AI 实现 —— 使用正式决策树框架
 *
 * 决策树结构：
 *   Root
 *   ├── FireIfAligned (射程300, 容差40)
 *   ├── CollectPowerUp
 *   └── Patrol
 *
 * AI 自主决定射击：当 FireIfAligned 返回 Fire 时，AI 调用 tryFire() 发射子弹。
 */

#include "patrol_ai.h"
#include "decision_tree.h"
#include "../core/enemy_tank.h"
#include "../core/game_config.h"
#include "../utils/factory.h"

PatrolAI::PatrolAI() {
    // 构建决策树
    m_root = std::make_unique<SelectorNode>();
    auto fireNode = std::make_unique<FireIfAlignedNode>(
        GameConfig::PATROL_AI_FIRE_RANGE,
        GameConfig::PATROL_AI_ALIGN_THRESHOLD);
    auto collectNode = std::make_unique<CollectPowerUpNode>();
    auto patrolNode = std::make_unique<PatrolNode>();

    m_root->addChild(std::move(fireNode));
    m_root->addChild(std::move(collectNode));
    m_root->addChild(std::move(patrolNode));
}

void PatrolAI::update(EnemyTank* tank, const GameState& state) {
    if (!tank || !tank->isAlive()) return;

    AIAction action = m_root->decide(tank, state);

    // AI 自主决定射击
    if (action == AIAction::Fire) {
        tryFire(tank);
    }

    // 如果玩家不在火线内，尝试先对齐方向再开火
    if (state.playerAlive && action != AIAction::Fire) {
        Direction toPlayer = directionTo(tank->position(), state.playerPos);
        if (isTargetInLineOfFire(tank, state.playerPos,
                                  GameConfig::PATROL_AI_FIRE_RANGE,
                                  GameConfig::PATROL_AI_ALIGN_THRESHOLD)) {
            tank->setDirection(toPlayer);
            tryFire(tank);
        }
    }
}

REGISTER_AI(PatrolAI);
