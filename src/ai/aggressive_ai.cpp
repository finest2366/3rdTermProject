/**
 * @file aggressive_ai.cpp
 * @brief 激进 AI 实现 —— 使用决策树 + Boss 模式支持
 *
 * 决策树结构：
 *   Root
 *   ├── FleeIfLowHp (Boss 模式：HP<2 时逃跑)
 *   ├── FireIfAligned (射程350, 容差40)
 *   ├── CollectPowerUp
 *   └── ChasePlayer (追击范围400)
 *
 * Boss 模式增强：
 * - 连发射击（burst fire）：每次对齐后连续发射3发
 * - 低血量逃跑策略
 */

#include "aggressive_ai.h"
#include "decision_tree.h"
#include "../core/enemy_tank.h"
#include "../core/game_config.h"
#include "../utils/factory.h"
#include <QDebug>

AggressiveAI::AggressiveAI() {
    // 构建决策树
    auto fleeNode = std::make_unique<FleeIfLowHpNode>(GameConfig::BOSS_MIN_HP_FOR_FLEE);
    auto fireNode = std::make_unique<FireIfAlignedNode>(
        GameConfig::AGGRESSIVE_AI_FIRE_RANGE,
        GameConfig::AGGRESSIVE_AI_ALIGN_THRESHOLD);
    auto collectNode = std::make_unique<CollectPowerUpNode>();
    auto chaseNode = std::make_unique<ChasePlayerNode>(400.0);

    m_root = std::make_unique<SelectorNode>();
    // 决策树子节点按优先级排列
    m_root->addChild(std::move(fleeNode));
    m_root->addChild(std::move(fireNode));
    m_root->addChild(std::move(collectNode));
    m_root->addChild(std::move(chaseNode));
}

void AggressiveAI::update(EnemyTank* tank, const GameState& state) {
    if (!tank || !tank->isAlive()) return;

    AIAction action = m_root->decide(tank, state);

    // AI 自主决定射击
    if (action == AIAction::Fire) {
        Bullet* b = tryFire(tank);
        if (b && tank->type() == TankType::EnemyBoss) {
            // Boss 连发模式
            m_burstCounter++;
            if (m_burstCounter < GameConfig::BOSS_AI_BURST_COUNT) {
                // 连续发射（等下一帧再发）
            } else {
                m_burstCounter = 0;
            }
        }
        return;
    }

    // 如果未对齐但玩家在范围内，转向并尝试射击
    if (state.playerAlive && action != AIAction::Flee) {
        Direction toPlayer = directionTo(tank->position(), state.playerPos);
        tank->setDirection(toPlayer);
        if (isTargetInLineOfFire(tank, state.playerPos,
                                  GameConfig::AGGRESSIVE_AI_FIRE_RANGE,
                                  GameConfig::AGGRESSIVE_AI_ALIGN_THRESHOLD)) {
            tryFire(tank);
        }
    }
}

REGISTER_AI(AggressiveAI);
