/**
 * @file bullet.cpp
 * @brief 子弹实现
 */

#include "bullet.h"

Bullet::Bullet(const QPointF& pos, Direction dir, int ownerIndex)
    : m_position(pos)
    , m_direction(dir)
    , m_ownerIndex(ownerIndex)
{
}

void Bullet::update() {
    if (!m_active) return;

    switch (m_direction) {
    case Direction::Up:    m_position.ry() -= m_speed; break;
    case Direction::Down:  m_position.ry() += m_speed; break;
    case Direction::Left:  m_position.rx() -= m_speed; break;
    case Direction::Right: m_position.rx() += m_speed; break;
    }
}
