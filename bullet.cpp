#include "bullet.h"
#include "tank.h"
#include <QGraphicsScene>
#include <QtMath>

Bullet::Bullet(qreal x, qreal y, Direction dir, qreal tankVx, qreal tankVy,
               Tank* owner, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_direction(dir)
    , m_vx(0), m_vy(0)
    , m_owner(owner)
    , m_active(true)
    , m_bounces(0)
{
    setPos(x, y);
    setZValue(2);

    // 计算主方向的基础速度
    qreal baseVx = 0, baseVy = 0;
    switch (dir) {
    case DIR_UP:    baseVy = -BULLET_SPEED; break;
    case DIR_DOWN:  baseVy =  BULLET_SPEED; break;
    case DIR_LEFT:  baseVx = -BULLET_SPEED; break;
    case DIR_RIGHT: baseVx =  BULLET_SPEED; break;
    }

    // 叠加坦克惯性（坦克横向移动影响子弹方向）
    qreal rawVx = baseVx + tankVx * INERTIA_FACTOR;
    qreal rawVy = baseVy + tankVy * INERTIA_FACTOR;

    // 归一化到 BULLET_SPEED，保证子弹速度恒定
    qreal mag = qSqrt(rawVx * rawVx + rawVy * rawVy);
    if (mag > 0.01) {
        m_vx = rawVx / mag * BULLET_SPEED;
        m_vy = rawVy / mag * BULLET_SPEED;
    } else {
        // 退化情况：按主方向
        m_vx = baseVx;
        m_vy = baseVy;
    }
}

QRectF Bullet::boundingRect() const
{
    qreal half = BULLET_SIZE / 2.0;
    // 包含发光效果的外扩像素，避免留下痕迹
    constexpr qreal glow = 2.0;
    return QRectF(-half - glow, -half - glow, BULLET_SIZE + glow * 2, BULLET_SIZE + glow * 2);
}

QPainterPath Bullet::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void Bullet::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // 金色子弹，带发光效果
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(GameColors::BULLET.lighter(150), 3));
    painter->drawEllipse(boundingRect().adjusted(-1, -1, 1, 1));

    painter->setBrush(GameColors::BULLET);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(boundingRect());
}

void Bullet::move()
{
    if (!m_active) return;

    setPos(x() + m_vx, y() + m_vy);

    // 超出边界自动销毁
    if (x() < 0 || x() > SCENE_WIDTH || y() < 0 || y() > SCENE_HEIGHT) {
        deactivate();
    }
}

void Bullet::deactivate()
{
    m_active = false;
    if (scene()) {
        scene()->removeItem(this);
    }
    // 不在这里 delete — 由 GameScene::removeDeadObjects() 统一清理
}

void Bullet::bounce(int wallRow, int wallCol)
{
    m_bounces++;

    // === 光的反射定律 ===
    // 计算墙壁单元格中心
    qreal wallCx = wallCol * TILE_SIZE + TILE_SIZE / 2.0;
    qreal wallCy = wallRow * TILE_SIZE + TILE_SIZE / 2.0;

    // 子弹在 X 轴和 Y 轴上穿透墙壁的深度
    qreal penX = TILE_SIZE / 2.0 + BULLET_SIZE / 2.0 - qAbs(x() - wallCx);
    qreal penY = TILE_SIZE / 2.0 + BULLET_SIZE / 2.0 - qAbs(y() - wallCy);

    if (penX < penY) {
        // 碰撞垂直面 → 水平速度分量反转
        m_vx = -m_vx;
    } else {
        // 碰撞水平面 → 垂直速度分量反转
        m_vy = -m_vy;
    }

    // 向新方向微移一步，避免卡在墙里
    setPos(x() + m_vx, y() + m_vy);

    if (m_bounces >= BULLET_MAX_BOUNCES) {
        deactivate();
    }
}
