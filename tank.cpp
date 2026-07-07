#include "tank.h"
#include "bullet.h"
#include <QtMath>

Tank::Tank(qreal x, qreal y, Direction dir, int hp, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_direction(dir)
    , m_hp(hp)
    , m_maxHp(hp)
    , m_shootCooldown(0)
{
    setPos(x, y);
    setZValue(1);
}

QRectF Tank::boundingRect() const
{
    qreal half = TANK_SIZE / 2.0;
    return QRectF(-half, -half, TANK_SIZE, TANK_SIZE);
}

QPainterPath Tank::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool Tank::tryMove(Direction dir, qreal speed)
{
    qreal dx = 0, dy = 0;
    switch (dir) {
    case DIR_UP:    dy = -speed; break;
    case DIR_DOWN:  dy =  speed; break;
    case DIR_LEFT:  dx = -speed; break;
    case DIR_RIGHT: dx =  speed; break;
    }

    qreal newX = x() + dx;
    qreal newY = y() + dy;

    // 边界检查 (坦克不能超出场景)
    qreal half = TANK_SIZE / 2.0;
    if (newX - half < 0 || newX + half > SCENE_WIDTH ||
        newY - half < 0 || newY + half > SCENE_HEIGHT) {
        return false;
    }

    // 移动成功才记录速度（用于子弹惯性）
    m_moveDx = dx;
    m_moveDy = dy;
    setPos(newX, newY);
    return true;
}

Bullet* Tank::shoot()
{
    if (!canShoot()) return nullptr;

    QPointF tip = barrelTip();
    Bullet* bullet = new Bullet(tip.x(), tip.y(), m_direction, m_moveDx, m_moveDy, nullptr);  // 传入坦克速度实现惯性
    m_shootCooldown = SHOOT_COOLDOWN;
    return bullet;
}

void Tank::takeDamage()
{
    m_hp--;
}

void Tank::tickCooldown()
{
    if (m_shootCooldown > 0)
        m_shootCooldown--;
}

void Tank::respawn(qreal x, qreal y)
{
    m_hp = m_maxHp;
    m_shootCooldown = 0;
    setPos(x, y);
    m_direction = DIR_UP;
    setVisible(true);
}

QPointF Tank::barrelTip() const
{
    qreal half = TANK_SIZE / 2.0 + 2;
    switch (m_direction) {
    case DIR_UP:    return QPointF(x(), y() - half);
    case DIR_DOWN:  return QPointF(x(), y() + half);
    case DIR_LEFT:  return QPointF(x() - half, y());
    case DIR_RIGHT: return QPointF(x() + half, y());
    }
    return QPointF(x(), y() - half);
}
