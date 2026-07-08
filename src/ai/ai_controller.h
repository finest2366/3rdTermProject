/**
 * @file ai_controller.h
 * @brief AI 控制器接口 —— 所有 AI 行为的策略基类
 *
 * 每个敌方坦克持有一个 AI 控制器实例。
 * 扩展方式：继承 AIController 实现 update()，使用 REGISTER_AI 宏注册。
 *
 * v2.0 增强：AI 自主决定开火（tryFire），而非依赖全局随机概率。
 */

#pragma once

#include "../core/game_common.h"
#include <QPointF>

class EnemyTank;
class Bullet;

class AIController {
public:
    AIController() = default;
    virtual ~AIController() = default;

    /**
     * @brief 每帧更新 AI 决策（移动 + 射击）
     * @param tank 控制的敌方坦克
     * @param state 当前游戏状态快照
     */
    virtual void update(EnemyTank* tank, const GameState& state) = 0;

    /** @brief AI 名称（用于识别和调试） */
    virtual QString name() const = 0;

    // ---- 公开的 AI 工具方法（供决策树节点等外部调用） ----

    /** @brief 曼哈顿距离 */
    static double manhattanDist(const QPointF& a, const QPointF& b);

    /** @brief 从 from 到 to 的方向 */
    static Direction directionTo(const QPointF& from, const QPointF& to);

    /** @brief 尝试开火，返回发射的子弹（nullptr=冷却中） */
    static Bullet* tryFire(EnemyTank* tank);

    /** @brief 检查目标是否在火线内 */
    static bool isTargetInLineOfFire(const EnemyTank* tank, const QPointF& targetPos,
                                      double fireRange, double alignThreshold);

protected:
};
