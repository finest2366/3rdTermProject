/**
 * @file powerup.h
 * @brief 道具系统 —— 基类 + 7种具体道具
 *
 * 扩展方式：继承 PowerUp 实现 apply()，使用 REGISTER_POWERUP 宏注册。
 * 工厂模式：PowerUpFactory::create("ClassName") 创建实例。
 *
 * 素材接口：每种道具对应一个资源 key，格式 "powerup_<类型名>"，
 * 可通过 data/custom_resources.json 替换图标。
 */

#pragma once

#include "game_common.h"
#include <QPointF>
#include <QSizeF>
#include <QString>

class Tank;

// ============================================================
// 道具基类
// ============================================================
class PowerUp {
public:
    PowerUp(PowerUpType type, const QString& name, const QPointF& pos, int durationMs = 0);
    virtual ~PowerUp() = default;

    /** @brief 将道具效果应用到指定坦克 */
    virtual void apply(Tank* tank) = 0;

    bool isActive() const { return m_active; }
    void collect() { m_active = false; }

    QPointF position() const { return m_position; }
    void setPosition(const QPointF& pos) { m_position = pos; }
    QSizeF size() const { return m_size; }
    PowerUpType powerUpType() const { return m_type; }
    const QString& name() const { return m_name; }
    int durationMs() const { return m_durationMs; }

    /** @brief 素材资源 key（用于 ResourceManager） */
    virtual QString resourceKey() const { return QString("powerup_%1").arg(m_name); }

protected:
    PowerUpType m_type;
    QString m_name;
    QPointF m_position;
    QSizeF m_size;
    int m_durationMs;  // 0 = 永久
    bool m_active = true;
};

// ============================================================
// 具体道具 (7种)
// ============================================================

class SpeedBoost : public PowerUp {
public:
    explicit SpeedBoost(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

class ShieldPowerUp : public PowerUp {
public:
    explicit ShieldPowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

class RapidFirePowerUp : public PowerUp {
public:
    explicit RapidFirePowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

class HealPowerUp : public PowerUp {
public:
    explicit HealPowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

class BombPowerUp : public PowerUp {
public:
    explicit BombPowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

// --- 新增道具 ---

/** @brief 冰冻 —— 冻结所有敌人5秒 */
class FreezePowerUp : public PowerUp {
public:
    explicit FreezePowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};

/** @brief 隐身 —— 敌人无法瞄准6秒 */
class InvisiblePowerUp : public PowerUp {
public:
    explicit InvisiblePowerUp(const QPointF& pos = QPointF(0, 0));
    void apply(Tank* tank) override;
};
