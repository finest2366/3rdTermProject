/**
 * @file player_tank.cpp
 * @brief 玩家坦克实现
 */

#include "player_tank.h"
#include "bullet.h"
#include <QKeyEvent>
#include <QtMath>

PlayerTank::PlayerTank(int playerIndex, const QPointF& pos)
    : Tank(TankType::Player1, QString("Player%1").arg(playerIndex + 1),
           pos, Direction::Up,
           GameConfig::PLAYER1_MAX_HP,
           GameConfig::PLAYER1_SPEED,
           GameConfig::PLAYER1_FIRE_COOLDOWN_MS)
{
    setPlayerIndex(playerIndex);

    if (playerIndex == 0) {
        // 玩家1: WASD + Space + Q(技能)
        m_keyUp    = Qt::Key_W;
        m_keyDown  = Qt::Key_S;
        m_keyLeft  = Qt::Key_A;
        m_keyRight = Qt::Key_D;
        m_keyFire  = Qt::Key_Space;
        m_keySkill = Qt::Key_Q;
        m_maxHp = GameConfig::PLAYER1_MAX_HP;
        m_hp = GameConfig::PLAYER1_MAX_HP;
    } else {
        // 玩家2: 方向键 + Enter + Shift(技能)
        m_keyUp    = Qt::Key_Up;
        m_keyDown  = Qt::Key_Down;
        m_keyLeft  = Qt::Key_Left;
        m_keyRight = Qt::Key_Right;
        m_keyFire  = Qt::Key_Return;
        m_keySkill = Qt::Key_Shift;
        m_maxHp = GameConfig::PLAYER2_MAX_HP;
        m_hp = GameConfig::PLAYER2_MAX_HP;
    }
}

Bullet* PlayerTank::fire() {
    if (!canFire()) return nullptr;

    resetFireCooldown();

    // 计算子弹生成位置（坦克前方中心）
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
                      m_direction, m_playerIndex);
}

Direction PlayerTank::handleKeyPress(int key) const {
    if (key == m_keyUp)    return Direction::Up;
    if (key == m_keyDown)  return Direction::Down;
    if (key == m_keyLeft)  return Direction::Left;
    if (key == m_keyRight) return Direction::Right;
    return m_direction;  // 不改变方向
}

bool PlayerTank::isFireKey(int key) const {
    return key == m_keyFire;
}

bool PlayerTank::isSkillKey(int key) const {
    return key == m_keySkill;
}
