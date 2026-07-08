/**
 * @file skill.h
 * @brief 技能基类 + 3种具体技能
 *
 * 与被动道具不同，技能需要玩家主动激活并有冷却时间。
 *
 * 素材接口：每种技能对应资源 key "skill_<技能名>"，
 * 可通过 data/custom_resources.json 替换技能图标。
 */

#pragma once

#include <QString>
#include <QElapsedTimer>

class Tank;
struct GameState;

// ============================================================
// 技能基类
// ============================================================
class Skill {
public:
    Skill(const QString& name, int cooldownMs);
    virtual ~Skill() = default;

    /** @brief 激活技能，返回是否成功（冷却就绪） */
    virtual bool activate(Tank* tank, const GameState& state) = 0;

    const QString& name() const { return m_name; }
    int cooldownMs() const { return m_cooldownMs; }
    bool isReady() const { return m_cooldownTimer.elapsed() >= m_cooldownMs; }
    void resetCooldown() { m_cooldownTimer.restart(); }

    int remainingCooldown() const {
        return qMax(0, m_cooldownMs - static_cast<int>(m_cooldownTimer.elapsed()));
    }

    float cooldownPercent() const {
        if (m_cooldownMs <= 0) return 1.0f;
        return qMin(1.0f, static_cast<float>(m_cooldownTimer.elapsed()) / m_cooldownMs);
    }

    /** @brief 素材资源 key */
    virtual QString resourceKey() const { return QString("skill_%1").arg(m_name); }

protected:
    QString m_name;
    int m_cooldownMs;
    QElapsedTimer m_cooldownTimer;
};

// ============================================================
// 具体技能 (3种)
// ============================================================

/** @brief 疾跑 —— 临时大幅提升移动速度 */
class SprintSkill : public Skill {
public:
    SprintSkill();
    bool activate(Tank* tank, const GameState& state) override;
};

/** @brief 导弹 —— 发射一枚可以穿越墙壁的导弹 */
class MissileSkill : public Skill {
public:
    MissileSkill();
    bool activate(Tank* tank, const GameState& state) override;
};

/** @brief 盾墙 —— 在坦克前方生成临时不可穿越的墙壁 */
class ShieldWallSkill : public Skill {
public:
    ShieldWallSkill();
    bool activate(Tank* tank, const GameState& state) override;
};
