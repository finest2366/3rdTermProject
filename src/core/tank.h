/**
 * @file tank.h
 * @brief 坦克基类 —— 所有坦克（玩家/敌方/Boss）的公共抽象
 *
 * 属性：位置、速度、生命值、方向、子弹冷却、所属阵营
 * 状态：护盾、连射、冰冻、隐身
 * 子类需实现 fire()；敌方坦克额外实现 updateAI()
 */

#pragma once

#include "game_common.h"
#include "game_config.h"
#include <QPointF>
#include <QSizeF>
#include <QElapsedTimer>
#include <QString>

class Bullet;
class Skill;

class Tank {
public:
    Tank(TankType type, const QString& name, const QPointF& pos, Direction dir,
         int hp, double speed, int fireCooldownMs);
    virtual ~Tank() = default;

    // ---- 纯虚函数 ----
    /** @brief 开火，生成一颗子弹。若冷却中返回 nullptr */
    virtual Bullet* fire() = 0;

    /** @brief AI 逻辑更新（默认空实现，仅敌方使用） */
    virtual void updateAI(const GameState& state) { Q_UNUSED(state); }

    // ---- 移动 ----
    void move(Direction dir);
    void stop();

    // ---- 属性访问 ----
    QPointF position() const { return m_position; }
    void setPosition(const QPointF& pos) { m_position = pos; }
    QSizeF size() const { return QSizeF(m_size, m_size); }
    double sizeValue() const { return m_size; }

    Direction direction() const { return m_direction; }
    void setDirection(Direction dir) { m_direction = dir; }

    double speed() const { return m_effectiveSpeed(); }
    double baseSpeed() const { return m_speed; }
    void setSpeed(double s) { m_speed = s; }

    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    void takeDamage(int dmg);
    void heal(int amount);
    bool isAlive() const { return m_hp > 0; }

    TankType type() const { return m_type; }
    const QString& name() const { return m_name; }
    bool isPlayer() const { return m_playerIndex >= 0; }
    int playerIndex() const { return m_playerIndex; }
    void setPlayerIndex(int idx) { m_playerIndex = idx; }

    // ---- 子弹冷却 ----
    bool canFire() const;
    void resetFireCooldown();
    int fireCooldownMs() const { return m_fireCooldownMs; }

    // ---- 护盾 ----
    bool hasShield() const { return m_hasShield; }
    void setShield(bool s) { m_hasShield = s; }

    // ---- 连射 ----
    bool hasRapidFire() const { return m_hasRapidFire; }
    void setRapidFire(bool rf) { m_hasRapidFire = rf; }
    int rapidFireTimer() const { return m_rapidFireTimer; }
    void tickRapidFire(int ms) { m_rapidFireTimer = std::max(0, m_rapidFireTimer - ms); }

    // ---- 冰冻 (新增) ----
    bool isFrozen() const { return m_isFrozen; }
    void setFrozen(bool f) { m_isFrozen = f; }

    // ---- 隐身 (新增) ----
    bool isInvisible() const { return m_isInvisible; }
    void setInvisible(bool inv) { m_isInvisible = inv; }

    // ---- 技能 (新增) ----
    Skill* skill() const { return m_skill.get(); }
    void setSkill(std::unique_ptr<Skill> skill);

protected:
    TankType m_type;
    QString m_name;
    QPointF m_position;
    double m_size = static_cast<double>(GameConfig::TANK_SIZE);
    Direction m_direction = Direction::Up;
    double m_speed = 2.0;               // 基础速度（像素/帧）
    int m_hp = 1;
    int m_maxHp = 1;
    int m_fireCooldownMs = 500;         // 射击冷却（毫秒）
    QElapsedTimer m_fireTimer;
    int m_playerIndex = -1;             // 0=玩家1, 1=玩家2, -1=敌方

    // 增益/减益状态
    bool m_hasShield = false;
    bool m_hasRapidFire = false;
    int m_rapidFireTimer = 0;
    bool m_isFrozen = false;            // 冰冻（无法移动和射击）
    bool m_isInvisible = false;         // 隐身（敌人无法瞄准）

    // 技能
    std::unique_ptr<Skill> m_skill;

private:
    /** @brief 计算实际速度（考虑所有增益/减益） */
    double m_effectiveSpeed() const {
        if (m_isFrozen) return 0.0;
        return m_speed;
    }
};
