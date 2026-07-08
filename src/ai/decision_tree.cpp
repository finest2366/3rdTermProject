/**
 * @file decision_tree.cpp
 * @brief 决策树框架实现
 */

#include "decision_tree.h"
#include "ai_controller.h"
#include "../core/enemy_tank.h"
#include "../core/game_config.h"
#include <QRandomGenerator>
#include <QtMath>

// ===== DecisionNode =====
void DecisionNode::addChild(std::unique_ptr<DecisionNode> child) {
    m_children.push_back(std::move(child));
}

AIAction DecisionNode::decide(EnemyTank* tank, const GameState& state) {
    // 先评估自身
    AIAction result = evaluate(tank, state);
    if (result != AIAction::None) {
        return result;
    }
    // 再按优先级评估子节点
    for (auto& child : m_children) {
        result = child->decide(tank, state);
        if (result != AIAction::None) {
            return result;
        }
    }
    return AIAction::None;
}

// ===== SelectorNode (根节点，始终返回 None) =====
AIAction SelectorNode::evaluate(EnemyTank* tank, const GameState& state) {
    Q_UNUSED(tank);
    Q_UNUSED(state);
    return AIAction::None;  // 仅遍历子节点
}

// ===== FireIfAlignedNode =====
FireIfAlignedNode::FireIfAlignedNode(double range, double threshold)
    : m_range(range), m_threshold(threshold)
{
}

AIAction FireIfAlignedNode::evaluate(EnemyTank* tank, const GameState& state) {
    if (!state.playerAlive) return AIAction::None;

    // 优先瞄准玩家1
    QPointF target = state.playerPos;
    if (AIController::isTargetInLineOfFire(tank, target, m_range, m_threshold)) {
        return AIAction::Fire;
    }
    // 双人模式也检查玩家2
    if (state.player2Alive) {
        if (AIController::isTargetInLineOfFire(tank, state.player2Pos, m_range, m_threshold)) {
            return AIAction::Fire;
        }
    }
    return AIAction::None;
}

// ===== CollectPowerUpNode =====
AIAction CollectPowerUpNode::evaluate(EnemyTank* tank, const GameState& state) {
    if (state.powerUpPositions.isEmpty()) return AIAction::None;

    // 找最近道具
    QPointF closest = state.powerUpPositions.first();
    double minDist = AIController::manhattanDist(tank->position(), closest);
    for (const auto& puPos : state.powerUpPositions) {
        double d = AIController::manhattanDist(tank->position(), puPos);
        if (d < minDist) {
            minDist = d;
            closest = puPos;
        }
    }
    tank->setDirection(AIController::directionTo(tank->position(), closest));
    tank->move(tank->direction());
    return AIAction::MoveToTarget;
}

// ===== ChasePlayerNode =====
ChasePlayerNode::ChasePlayerNode(double chaseRange)
    : m_chaseRange(chaseRange)
{
}

AIAction ChasePlayerNode::evaluate(EnemyTank* tank, const GameState& state) {
    if (!state.playerAlive) return AIAction::None;

    double dist = AIController::manhattanDist(tank->position(), state.playerPos);
    if (dist < m_chaseRange) {
        Direction dir = AIController::directionTo(tank->position(), state.playerPos);
        tank->setDirection(dir);
        tank->move(dir);
        return AIAction::MoveToTarget;
    }
    return AIAction::None;
}

// ===== PatrolNode =====
PatrolNode::PatrolNode() {
    m_step = QRandomGenerator::global()->bounded(8);
}

AIAction PatrolNode::evaluate(EnemyTank* tank, const GameState& state) {
    Q_UNUSED(state);
    m_stepsInDir++;

    // 每 60 步换方向
    if (m_stepsInDir >= GameConfig::PATROL_AI_MAX_STEPS_PER_DIR) {
        m_stepsInDir = 0;
        m_step = QRandomGenerator::global()->bounded(8);
    }

    static const AIAction patrolActions[] = {
        AIAction::MoveDown, AIAction::MoveDown,
        AIAction::MoveRight, AIAction::MoveRight,
        AIAction::MoveUp, AIAction::MoveUp,
        AIAction::MoveLeft, AIAction::MoveLeft
    };

    AIAction action = patrolActions[m_step % 8];
    int dir = actionToDirection(action);
    if (dir >= 0) {
        tank->setDirection(static_cast<Direction>(dir));
        tank->move(static_cast<Direction>(dir));
    }
    return action;
}

// ===== FleeIfLowHpNode =====
FleeIfLowHpNode::FleeIfLowHpNode(int hpThreshold)
    : m_hpThreshold(hpThreshold)
{
}

AIAction FleeIfLowHpNode::evaluate(EnemyTank* tank, const GameState& state) {
    if (tank->hp() > m_hpThreshold) return AIAction::None;
    if (!state.playerAlive) return AIAction::None;

    // 朝远离玩家的方向移动
    Direction toPlayer = AIController::directionTo(tank->position(), state.playerPos);
    Direction fleeDir = oppositeDirection(toPlayer);
    tank->setDirection(fleeDir);
    tank->move(fleeDir);
    return AIAction::Flee;
}
