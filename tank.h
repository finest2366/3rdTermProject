#ifndef TANK_H
#define TANK_H

#include <QGraphicsItem>
#include <QPainter>
#include "constants.h"

class Bullet;

class Tank : public QGraphicsItem
{
public:
    Tank(qreal x, qreal y, Direction dir, int hp, QGraphicsItem* parent = nullptr);

    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // 移动和射击
    virtual bool tryMove(Direction dir, qreal speed = TANK_SPEED);
    virtual Bullet* shoot();

    // 伤害
    void takeDamage();
    bool isAlive() const { return m_hp > 0; }
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }

    // 属性
    Direction direction() const { return m_direction; }
    void setDirection(Direction dir) { m_direction = dir; }
    bool canShoot() const { return m_shootCooldown <= 0; }
    int shootCooldown() const { return m_shootCooldown; }
    void resetCooldown() { m_shootCooldown = SHOOT_COOLDOWN; }
    void tickCooldown();
    void respawn(qreal x, qreal y);

    // 获取炮管尖端位置 (子弹发射点)
    QPointF barrelTip() const;

    // 惯性：当前帧的移动速度（供子弹继承）
    qreal moveDx() const { return m_moveDx; }
    qreal moveDy() const { return m_moveDy; }

protected:
    Direction m_direction;
    int m_hp;
    int m_maxHp;
    int m_shootCooldown;
    qreal m_moveDx = 0;
    qreal m_moveDy = 0;
};

#endif // TANK_H
