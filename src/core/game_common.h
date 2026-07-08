/**
 * @file game_common.h
 * @brief 游戏公共类型定义 —— 枚举、结构体、前向声明
 *
 * 所有模块共享的基础类型集中于此文件。
 * 修改此文件会影响整个项目，请谨慎操作。
 */

#pragma once

#include <QObject>
#include <QPointF>
#include <QSizeF>
#include <QVector>
#include <QString>

// ============================================================
// 前向声明
// ============================================================
class Tank;
class Bullet;
class Wall;
class Base;
class PowerUp;

// ============================================================
// 方向枚举
// ============================================================
enum class Direction {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

/** @brief 方向 → 字符串（调试用） */
inline const char* directionName(Direction d) {
    switch (d) {
    case Direction::Up:    return "Up";
    case Direction::Right: return "Right";
    case Direction::Down:  return "Down";
    case Direction::Left:  return "Left";
    }
    return "Unknown";
}

/** @brief 返回相反方向 */
inline Direction oppositeDirection(Direction d) {
    return static_cast<Direction>((static_cast<int>(d) + 2) % 4);
}

// ============================================================
// 坦克类型
// ============================================================
enum class TankType {
    Player1,      // 玩家1
    Player2,      // 玩家2 (双人模式)
    EnemyBasic,   // 普通敌人
    EnemyFast,    // 快速敌人
    EnemyHeavy,   // 重装敌人
    EnemyBoss     // Boss 敌人 (新增)
};

// ============================================================
// 道具类型
// ============================================================
enum class PowerUpType {
    Speed,        // 加速
    Shield,       // 护盾
    RapidFire,    // 连射
    Heal,         // 回血
    Bomb,         // 全屏炸弹
    Freeze,       // 冰冻 (新增: 冻结所有敌人)
    Invisible     // 隐身 (新增: 敌人无法瞄准)
};

// ============================================================
// 墙壁类型
// ============================================================
enum class WallType {
    Brick,        // 砖墙（可摧毁）
    Steel,        // 钢铁（不可摧毁）
    Water         // 水域（不可穿越但子弹可通过，新增）
};

// ============================================================
// 音效 ID
// ============================================================
enum class SoundId {
    Shoot,
    Explosion,
    PowerUp,
    BGM,
    MenuClick,
    GameOver,
    Freeze,       // 冰冻音效
    Invisible     // 隐身音效
};

// ============================================================
// 游戏状态快照 —— 供 AI 控制器读取
// ============================================================
struct GameState {
    // 玩家1 信息
    QPointF playerPos;
    Direction playerDir = Direction::Up;
    bool playerAlive = true;
    bool playerHasShield = false;

    // 玩家2 信息 (双人模式)
    QPointF player2Pos;
    bool player2Alive = false;

    // 敌方信息
    QVector<QPointF> enemyPositions;
    QVector<Direction> enemyDirections;

    // 子弹信息
    QVector<QPointF> bulletPositions;
    QVector<bool> bulletIsPlayerOwned;  // true = 玩家子弹

    // 道具信息
    QVector<QPointF> powerUpPositions;
    QVector<PowerUpType> powerUpTypes;

    // 基地信息
    QPointF basePosition;
    bool baseAlive = true;

    // 地图信息
    QVector<QPointF> wallPositions;     // 所有墙壁位置
    int mapWidth = 600;
    int mapHeight = 600;
};

// ============================================================
// AI 决策结果 —— 供决策树节点返回
// ============================================================
enum class AIAction {
    None,          // 无操作
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Fire,          // 开火
    MoveToTarget,  // 移向目标
    Patrol,        // 巡逻
    Flee           // 逃跑
};

/** @brief 将 AIAction 转换为 Direction，失败返回 -1 */
inline int actionToDirection(AIAction action) {
    switch (action) {
    case AIAction::MoveUp:    return 0;
    case AIAction::MoveRight: return 1;
    case AIAction::MoveDown:  return 2;
    case AIAction::MoveLeft:  return 3;
    default: return -1;
    }
}
