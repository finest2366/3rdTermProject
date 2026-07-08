/**
 * @file decision_tree.h
 * @brief 轻量级决策树框架 —— 用于 AI 行为决策
 *
 * 使用方式：
 *   1. 创建 DecisionNode 子类实现 evaluate()
 *   2. 通过 addChild() 构建树结构
 *   3. 调用 root->decide() 获取最终 AIAction
 *
 * 每个节点根据条件返回一个 AIAction，或委托给子节点。
 *
 * 素材接口：AI 决策树可通过 JSON 配置文件定义行为优先级，
 * 参见 data/ai_config.json（可选）。
 */

#pragma once

#include "../core/game_common.h"
#include <vector>
#include <memory>
#include <functional>

class EnemyTank;
struct GameState;

// ============================================================
// 决策节点基类
// ============================================================
class DecisionNode {
public:
    DecisionNode() = default;
    virtual ~DecisionNode() = default;

    /**
     * @brief 评估此节点
     * @param tank 当前坦克
     * @param state 游戏状态
     * @return 决策结果。若返回 AIAction::None 则继续评估子节点
     */
    virtual AIAction evaluate(EnemyTank* tank, const GameState& state) = 0;

    /** @brief 节点名称（调试用） */
    virtual QString nodeName() const = 0;

    /** @brief 添加子节点（按优先级顺序） */
    void addChild(std::unique_ptr<DecisionNode> child);

    /**
     * @brief 从当前节点开始决策，遍历所有子节点直到获得非 None 结果
     */
    AIAction decide(EnemyTank* tank, const GameState& state);

protected:
    std::vector<std::unique_ptr<DecisionNode>> m_children;
};

/**
 * @brief 选择器节点 —— 作为决策树根节点使用
 *
 * 自身永远返回 AIAction::None，只遍历子节点。
 * 用于构建优先级链：子节点按注册顺序评估，返回第一个非 None 结果。
 */
class SelectorNode : public DecisionNode {
public:
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "Selector"; }
};

// ============================================================
// 内置决策节点
// ============================================================

/**
 * @brief 条件节点：检测玩家是否在火线内 → 对齐并开火
 */
class FireIfAlignedNode : public DecisionNode {
public:
    FireIfAlignedNode(double range = 300.0, double threshold = 40.0);
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "FireIfAligned"; }
private:
    double m_range;
    double m_threshold;
};

/**
 * @brief 条件节点：检测是否有可见道具 → 移向最近道具
 */
class CollectPowerUpNode : public DecisionNode {
public:
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "CollectPowerUp"; }
};

/**
 * @brief 条件节点：检测玩家距离 → 追击
 */
class ChasePlayerNode : public DecisionNode {
public:
    ChasePlayerNode(double chaseRange = 400.0);
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "ChasePlayer"; }
private:
    double m_chaseRange;
};

/**
 * @brief 动作节点：随机巡逻（始终返回巡逻方向）
 */
class PatrolNode : public DecisionNode {
public:
    PatrolNode();
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "Patrol"; }
private:
    int m_step = 0;
    int m_stepsInDir = 0;
};

/**
 * @brief 条件节点：低血量时逃跑
 */
class FleeIfLowHpNode : public DecisionNode {
public:
    FleeIfLowHpNode(int hpThreshold = 2);
    AIAction evaluate(EnemyTank* tank, const GameState& state) override;
    QString nodeName() const override { return "FleeIfLowHp"; }
private:
    int m_hpThreshold;
};
