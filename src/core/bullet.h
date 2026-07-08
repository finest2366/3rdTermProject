/**
 * @file bullet.h
 * @brief 子弹类 —— 由坦克发射，沿方向直线飞行
 *
 * ownerIndex: -1=敌方, 0=玩家1, 1=玩家2
 */

#pragma once

#include "game_common.h"
#include "game_config.h"
#include <QPointF>
#include <QSizeF>

class Bullet {
public:
    Bullet(const QPointF& pos, Direction dir, int ownerIndex);
    ~Bullet() = default;

    /** @brief 每帧更新位置 */
    void update();

    QPointF position() const { return m_position; }
    void setPosition(const QPointF& p) { m_position = p; }
    QSizeF size() const { return QSizeF(m_radius * 2, m_radius * 2); }
    Direction direction() const { return m_direction; }
    double radius() const { return m_radius; }

    int ownerIndex() const { return m_ownerIndex; }
    bool isPlayerBullet() const { return m_ownerIndex >= 0; }
    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }

    double speed() const { return m_speed; }
    static double defaultSize() { return GameConfig::BULLET_SIZE; }

private:
    QPointF m_position;
    Direction m_direction;
    int m_ownerIndex;   // -1=敌方, 0=P1, 1=P2
    double m_speed = GameConfig::BULLET_SPEED;
    double m_radius = GameConfig::BULLET_RADIUS;
    bool m_active = true;
};
