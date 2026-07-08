/**
 * @file powerup.cpp
 * @brief 道具系统实现 —— 7种道具 + 工厂自动注册
 *
 * 素材接口说明：
 *   每种道具的素材 key 格式为 "powerup_<道具名>"，例如：
 *   - "powerup_Speed Boost" → 对应 assets/images/powerups/speed.png
 *   - "powerup_Shield"      → 对应 assets/images/powerups/shield.png
 *   用户可通过 data/custom_resources.json 替换这些素材路径。
 */

#include "powerup.h"
#include "tank.h"
#include "game_config.h"
#include "utils/factory.h"

// ===== 基类 =====
PowerUp::PowerUp(PowerUpType type, const QString& name, const QPointF& pos, int durationMs)
    : m_type(type)
    , m_name(name)
    , m_position(pos)
    , m_size(QSizeF(GameConfig::POWERUP_SIZE, GameConfig::POWERUP_SIZE))
    , m_durationMs(durationMs)
{
}

// ===== SpeedBoost =====
SpeedBoost::SpeedBoost(const QPointF& pos)
    : PowerUp(PowerUpType::Speed, QStringLiteral("Speed Boost"), pos,
               GameConfig::SPEED_BOOST_DURATION_MS)
{
}

void SpeedBoost::apply(Tank* tank) {
    if (tank) {
        tank->setSpeed(tank->baseSpeed() * GameConfig::SPEED_BOOST_MULTIPLIER);
    }
}
REGISTER_POWERUP(SpeedBoost);

// ===== ShieldPowerUp =====
ShieldPowerUp::ShieldPowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::Shield, QStringLiteral("Shield"), pos, 0)  // 0 = 永久直到被消耗
{
}

void ShieldPowerUp::apply(Tank* tank) {
    if (tank) {
        tank->setShield(true);
    }
}
REGISTER_POWERUP(ShieldPowerUp);

// ===== RapidFirePowerUp =====
RapidFirePowerUp::RapidFirePowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::RapidFire, QStringLiteral("Rapid Fire"), pos,
               GameConfig::RAPID_FIRE_DURATION_MS)
{
}

void RapidFirePowerUp::apply(Tank* tank) {
    if (tank) {
        tank->setRapidFire(true);
    }
}
REGISTER_POWERUP(RapidFirePowerUp);

// ===== HealPowerUp =====
HealPowerUp::HealPowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::Heal, QStringLiteral("Heal"), pos, 0)
{
}

void HealPowerUp::apply(Tank* tank) {
    if (tank) {
        tank->heal(GameConfig::HEAL_AMOUNT);
    }
}
REGISTER_POWERUP(HealPowerUp);

// ===== BombPowerUp =====
BombPowerUp::BombPowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::Bomb, QStringLiteral("Bomb"), pos, 0)
{
}

void BombPowerUp::apply(Tank* tank) {
    Q_UNUSED(tank);
    // 炸弹效果在 GameEngine 中处理（摧毁所有敌方坦克）
}
REGISTER_POWERUP(BombPowerUp);

// ===== FreezePowerUp (新增) =====
FreezePowerUp::FreezePowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::Freeze, QStringLiteral("Freeze"), pos,
               GameConfig::FREEZE_DURATION_MS)
{
}

void FreezePowerUp::apply(Tank* tank) {
    Q_UNUSED(tank);
    // 冰冻效果在 GameEngine 中处理（冻结所有敌人）
}
REGISTER_POWERUP(FreezePowerUp);

// ===== InvisiblePowerUp (新增) =====
InvisiblePowerUp::InvisiblePowerUp(const QPointF& pos)
    : PowerUp(PowerUpType::Invisible, QStringLiteral("Invisible"), pos,
               GameConfig::INVISIBLE_DURATION_MS)
{
}

void InvisiblePowerUp::apply(Tank* tank) {
    if (tank) {
        tank->setInvisible(true);
    }
}
REGISTER_POWERUP(InvisiblePowerUp);
