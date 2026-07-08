/**
 * @file skill.cpp
 * @brief 技能系统实现
 */

#include "skill.h"
#include "tank.h"
#include "game_config.h"
#include "game_common.h"
#include <QtMath>

// ===== 基类 =====
Skill::Skill(const QString& name, int cooldownMs)
    : m_name(name)
    , m_cooldownMs(cooldownMs)
{
    m_cooldownTimer.start();
}

// ===== SprintSkill (疾跑) =====
SprintSkill::SprintSkill()
    : Skill(QStringLiteral("Sprint"), GameConfig::SKILL_SPRINT_COOLDOWN_MS)
{
}

bool SprintSkill::activate(Tank* tank, const GameState& state) {
    Q_UNUSED(state);
    if (!isReady() || !tank || !tank->isAlive()) return false;

    tank->setSpeed(tank->baseSpeed() * GameConfig::SKILL_SPRINT_SPEED_MULT);
    resetCooldown();

    // 注意：速度恢复由 GameEngine 在 duration 后处理
    return true;
}

// ===== MissileSkill (导弹) =====
MissileSkill::MissileSkill()
    : Skill(QStringLiteral("Missile"), GameConfig::SKILL_MISSILE_COOLDOWN_MS)
{
}

bool MissileSkill::activate(Tank* tank, const GameState& state) {
    Q_UNUSED(state);
    if (!isReady() || !tank || !tank->isAlive()) return false;

    resetCooldown();
    // 导弹发射由 GameEngine 处理（生成穿透性子弹）
    return true;
}

// ===== ShieldWallSkill (盾墙) =====
ShieldWallSkill::ShieldWallSkill()
    : Skill(QStringLiteral("Shield Wall"), GameConfig::SKILL_SHIELD_WALL_COOLDOWN_MS)
{
}

bool ShieldWallSkill::activate(Tank* tank, const GameState& state) {
    Q_UNUSED(state);
    if (!isReady() || !tank || !tank->isAlive()) return false;

    resetCooldown();
    // 盾墙生成由 GameEngine 处理（在坦克前方生成临时墙壁）
    return true;
}
