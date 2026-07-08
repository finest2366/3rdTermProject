/**
 * @file enemy_tank.cpp
 * @brief 敌方坦克实现 —— 支持 Basic/Fast/Heavy/Boss 四种类型
 */

#include "enemy_tank.h"
#include "bullet.h"
#include "ai/ai_controller.h"
#include <QtMath>

EnemyTank::EnemyTank(TankType enemyType, const QPointF& pos, std::unique_ptr<AIController> ai)
    : Tank(enemyType, "Enemy", pos, Direction::Down,
           GameConfig::ENEMY_BASIC_HP,
           GameConfig::ENEMY_BASIC_SPEED,
           GameConfig::ENEMY_BASIC_FIRE_COOLDOWN_MS)
    , m_ai(std::move(ai))
{
    // 根据敌人类型覆写属性
    switch (enemyType) {
    case TankType::EnemyFast:
        m_name = QStringLiteral("Fast Enemy");
        m_speed = GameConfig::ENEMY_FAST_SPEED;
        m_hp = GameConfig::ENEMY_FAST_HP;
        m_maxHp = GameConfig::ENEMY_FAST_HP;
        m_fireCooldownMs = GameConfig::ENEMY_FAST_FIRE_COOLDOWN_MS;
        break;
    case TankType::EnemyHeavy:
        m_name = QStringLiteral("Heavy Enemy");
        m_speed = GameConfig::ENEMY_HEAVY_SPEED;
        m_hp = GameConfig::ENEMY_HEAVY_HP;
        m_maxHp = GameConfig::ENEMY_HEAVY_HP;
        m_fireCooldownMs = GameConfig::ENEMY_HEAVY_FIRE_COOLDOWN_MS;
        break;
    case TankType::EnemyBoss:
        m_name = QStringLiteral("BOSS");
        m_speed = GameConfig::ENEMY_BOSS_SPEED;
        m_hp = GameConfig::ENEMY_BOSS_HP;
        m_maxHp = GameConfig::ENEMY_BOSS_HP;
        m_fireCooldownMs = GameConfig::ENEMY_BOSS_FIRE_COOLDOWN_MS;
        m_size = 48.0;  // Boss 更大
        break;
    default:  // EnemyBasic
        m_name = QStringLiteral("Basic Enemy");
        m_speed = GameConfig::ENEMY_BASIC_SPEED;
        m_hp = GameConfig::ENEMY_BASIC_HP;
        m_maxHp = GameConfig::ENEMY_BASIC_HP;
        m_fireCooldownMs = GameConfig::ENEMY_BASIC_FIRE_COOLDOWN_MS;
        break;
    }
}

Bullet* EnemyTank::fire() {
    if (!canFire()) return nullptr;

    resetFireCooldown();

    double cx = m_position.x() + m_size / 2.0;
    double cy = m_position.y() + m_size / 2.0;
    double offset = m_size / 2.0 + 4.0;

    switch (m_direction) {
    case Direction::Up:    cy -= offset; break;
    case Direction::Down:  cy += offset; break;
    case Direction::Left:  cx -= offset; break;
    case Direction::Right: cx += offset; break;
    }

    return new Bullet(QPointF(cx - Bullet::defaultSize() / 2,
                               cy - Bullet::defaultSize() / 2),
                      m_direction, -1);  // -1 = 敌方子弹
}

void EnemyTank::updateAI(const GameState& state) {
    if (m_ai) {
        m_ai->update(this, state);
    }
}
