#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsItem>
#include <QPainter>
#include "constants.h"

class Tank;  // 前向声明

class Bullet : public QGraphicsItem
{
public:
    Bullet(qreal x, qreal y, Direction dir, qreal tankVx, qreal tankVy,
           Tank* owner, QGraphicsItem* parent = nullptr);

    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QPainterPath shape() const override;

    // 属性和方法
    void move();
    Direction direction() const { return m_direction; }
    qreal velocityX() const { return m_vx; }
    qreal velocityY() const { return m_vy; }
    Tank* owner() const { return m_owner; }
    bool isActive() const { return m_active; }
    void deactivate();
    int bounces() const { return m_bounces; }
    void bounce(int wallRow, int wallCol);  // 反弹：根据碰撞面反射速度分量

private:
    Direction m_direction;    // 射击朝向（主方向）
    qreal m_vx, m_vy;        // 实际速度分量（含惯性）
    Tank* m_owner;            // 发射者 (用于避免击中自己)
    bool m_active;
    int m_bounces;            // 已反弹次数
};

#endif // BULLET_H
