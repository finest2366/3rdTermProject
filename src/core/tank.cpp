/**
 * @file tank.cpp
 * @brief 坦克基类实现
 */

#include "tank.h"
#include "bullet.h"
#include "skill.h"
#include <QtMath>
#include <QDebug>

Tank::Tank(TankType type, const QString& name, const QPointF& pos, Direction dir,
           int hp, double speed, int fireCooldownMs)
    : m_type(type)
    , m_name(name)
    , m_position(pos)
    , m_direction(dir)
    , m_speed(speed)
    , m_hp(hp)
    , m_maxHp(hp)
    , m_fireCooldownMs(fireCooldownMs)
{
    m_fireTimer.start();
}

void Tank::move(Direction dir) {
    if (m_isFrozen) return;  // 冰冻状态无法移动
    m_direction = dir;
    double dx = 0, dy = 0;
    double spd = m_effectiveSpeed();
    switch (dir) {
    case Direction::Up:    dy = -spd; break;
    case Direction::Down:  dy =  spd; break;
    case Direction::Left:  dx = -spd; break;
    case Direction::Right: dx =  spd; break;
    }
    m_position += QPointF(dx, dy);
}

void Tank::stop() {
    // 坦克默认不持续移动
}

void Tank::takeDamage(int dmg) {
    if (m_hasShield) {
        m_hasShield = false;
        return;  // 护盾吸收一次伤害
    }
    m_hp -= dmg;
    if (m_hp < 0) m_hp = 0;
}

void Tank::heal(int amount) {
    m_hp = qMin(m_hp + amount, m_maxHp);
}

bool Tank::canFire() const {
    if (m_isFrozen) return false;  // 冰冻状态无法射击
    int cooldown = m_fireCooldownMs;
    if (m_hasRapidFire) {
        cooldown /= GameConfig::RAPID_FIRE_COOLDOWN_DIVISOR;
    }
    return m_fireTimer.elapsed() >= cooldown;
}

void Tank::resetFireCooldown() {
    m_fireTimer.restart();
}

void Tank::setSkill(std::unique_ptr<Skill> skill) {
    m_skill = std::move(skill);
}
