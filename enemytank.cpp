#include "enemytank.h"
#include "bullet.h"
#include "wall.h"
#include <QGraphicsScene>
#include <QtMath>
#include <cstdlib>
#include <ctime>

EnemyTank::EnemyTank(qreal x, qreal y, Difficulty diff, QGraphicsItem* parent)
    : Tank(x, y, DIR_DOWN, (diff == HARD ? 2 : 1), parent)
    , m_difficulty(diff)
    , m_moveDir(DIR_DOWN)
    , m_directionTimer(0)
    , m_shootDecisionTimer(0)
{
    // 根据难度设置参数
    switch (diff) {
    case EASY:
        m_dirChangeInterval = 60;
        m_shootInterval = 80;      // 射击间隔（不频繁）
        m_speed = 2.0;
        break;
    case NORMAL:
        m_dirChangeInterval = 45;
        m_shootInterval = 60;      // 中等频率
        m_speed = 2.5;
        break;
    case HARD:
        m_dirChangeInterval = 30;
        m_shootInterval = 40;      // 有节制
        m_speed = 3.0;
        break;
    }

    // 随机初始化计时器
    m_directionTimer = rand() % m_dirChangeInterval;
    m_shootDecisionTimer = rand() % m_shootInterval;
}

void EnemyTank::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // 根据难度选择颜色深浅
    QColor bodyColor = GameColors::ENEMY_BODY;
    QColor barrelColor = GameColors::ENEMY_BARREL;
    if (m_difficulty == HARD) {
        bodyColor = bodyColor.darker(130);
        barrelColor = barrelColor.darker(130);
    } else if (m_difficulty == EASY) {
        bodyColor = bodyColor.lighter(120);
    }

    QRectF rect = boundingRect();

    // 绘制车身
    painter->setBrush(bodyColor);
    painter->setPen(QPen(bodyColor.darker(150), 2));
    painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 6, 6);

    // 绘制履带装饰
    painter->setPen(QPen(bodyColor.darker(180), 3));
    qreal hw = TANK_SIZE / 2.0;
    qreal hh = TANK_SIZE / 2.0;
    if (m_direction == DIR_UP || m_direction == DIR_DOWN) {
        painter->drawLine(QPointF(-hw + 5, -hh), QPointF(-hw + 5, hh));
        painter->drawLine(QPointF( hw - 5, -hh), QPointF( hw - 5, hh));
    } else {
        painter->drawLine(QPointF(-hw, -hh + 5), QPointF(hw, -hh + 5));
        painter->drawLine(QPointF(-hw,  hh - 5), QPointF(hw,  hh - 5));
    }

    // 绘制炮管
    painter->setBrush(barrelColor);
    painter->setPen(QPen(barrelColor.darker(150), 1));
    qreal bw = 6;
    qreal bl = TANK_SIZE / 2.0 + 4;
    QRectF barrelRect;
    switch (m_direction) {
    case DIR_UP:
        barrelRect = QRectF(-bw/2, -bl, bw, bl);
        break;
    case DIR_DOWN:
        barrelRect = QRectF(-bw/2, hh - 4, bw, bl);
        break;
    case DIR_LEFT:
        barrelRect = QRectF(-bl, -bw/2, bl, bw);
        break;
    case DIR_RIGHT:
        barrelRect = QRectF(hw - 4, -bw/2, bl, bw);
        break;
    }
    painter->drawRect(barrelRect);

    // 炮塔
    painter->setBrush(bodyColor.lighter(130));
    painter->setPen(QPen(bodyColor.darker(120), 1));
    painter->drawEllipse(QPointF(0, 0), 8, 8);

    // HP 显示
    if (m_hp > 0) {
        painter->setBrush(Qt::white);
        painter->setPen(Qt::NoPen);
        for (int i = 0; i < m_hp; i++) {
            painter->drawEllipse(QPointF(-10 + i * 10, -hw + 10), 3, 3);
        }
    }
}

void EnemyTank::updateAI(const QPointF& playerPos)
{
    // 每帧重置惯性速度
    m_moveDx = 0;
    m_moveDy = 0;

    m_directionTimer--;
    m_shootDecisionTimer--;

    // ===== 1. 躲避子弹（最高优先级） =====
    bool dodging = tryDodgeBullets();

    // ===== 2. 移动决策 =====
    if (!dodging && m_directionTimer <= 0) {
        m_directionTimer = m_dirChangeInterval + (rand() % 15);

        int r = rand() % 100;
        if (r < 35) {
            // 35%：向玩家方向移动
            m_moveDir = directionTowardPlayer(playerPos);
        } else if (r < 60) {
            // 25%：向下移动（逼近基地）
            m_moveDir = DIR_DOWN;
        } else if (r < 70) {
            // 10%：保持当前方向
        } else {
            // 30%：随机变向
            changeRandomDirection();
        }
    }

    // 面朝移动方向（经典坦克大战风格）
    m_direction = m_moveDir;

    bool moved = tryMove(m_moveDir, m_speed);
    if (!moved) {
        onBlocked();
    }

    // ===== 3. 射击：只在有意义时开火 =====
    if (m_shootDecisionTimer <= 0) {
        m_shootDecisionTimer = m_shootInterval + (rand() % 12);

        // 判断玩家是否在射程和大致方向上
        Direction toPlayer = directionTowardPlayer(playerPos);
        bool playerAhead = (toPlayer == m_direction);
        qreal distToPlayer = qSqrt(
            (playerPos.x() - x()) * (playerPos.x() - x()) +
            (playerPos.y() - y()) * (playerPos.y() - y())
        );

        // 只有玩家在前方 或 距离较近时才考虑射击
        bool wantToShoot = playerAhead || (distToPlayer < TILE_SIZE * 5);

        if (wantToShoot && (rand() % 100) < 65) {
            // 短暂瞄准玩家方向再开火
            m_direction = toPlayer;

            if (canShoot() && !isWallInFront(playerPos)) {
                Bullet* bullet = shoot();
                if (bullet && scene()) {
                    scene()->addItem(bullet);
                }
            }
        }
    }

    tickCooldown();
}

bool EnemyTank::tryDodgeBullets()
{
    if (!scene()) return false;

    // 根据难度的闪避概率
    int dodgeChance;
    switch (m_difficulty) {
    case EASY:   dodgeChance = 40; break;
    case NORMAL: dodgeChance = 60; break;
    case HARD:   dodgeChance = 75; break;
    default:     dodgeChance = 50; break;
    }

    // 扫描周围 2 格范围内的子弹
    qreal scanRange = TILE_SIZE * 2;
    QRectF scanRect(x() - scanRange, y() - scanRange, scanRange * 2, scanRange * 2);
    QList<QGraphicsItem*> nearby = scene()->items(scanRect);

    Bullet* threat = nullptr;
    qreal threatDist = scanRange;

    for (auto* item : nearby) {
        Bullet* bullet = dynamic_cast<Bullet*>(item);
        if (!bullet || !bullet->isActive()) continue;
        if (bullet->owner() == this) continue;
        if (dynamic_cast<EnemyTank*>(bullet->owner())) continue;

        qreal bx = bullet->x(), by = bullet->y();
        qreal bvx = bullet->velocityX(), bvy = bullet->velocityY();
        qreal dist = qSqrt((bx - x()) * (bx - x()) + (by - y()) * (by - y()));

        // 子弹太远不管
        if (dist > TILE_SIZE * 2) continue;

        // 预测下一帧距离
        qreal nextDist = qSqrt(
            (bx + bvx - x()) * (bx + bvx - x()) +
            (by + bvy - y()) * (by + bvy - y())
        );

        // 子弹在明显靠近（至少接近了 3px）→ 威胁
        if (nextDist < dist - 3.0 && dist < threatDist) {
            threat = bullet;
            threatDist = dist;
        }
    }

    if (!threat) return false;

    // 概率判定：不是每次都躲
    if ((rand() % 100) >= dodgeChance) return false;

    // 向子弹轨迹的垂直方向闪避
    qreal bvx = threat->velocityX();
    qreal bvy = threat->velocityY();

    if (qAbs(bvx) > qAbs(bvy)) {
        m_moveDir = (y() > threat->y()) ? DIR_DOWN : DIR_UP;
    } else {
        m_moveDir = (x() > threat->x()) ? DIR_RIGHT : DIR_LEFT;
    }

    return true;
}

void EnemyTank::onBlocked()
{
    // 撞墙后选择新方向，避免立即反向（防止来回摆动）
    Direction blocked = m_moveDir;
    Direction opposite;
    switch (blocked) {
    case DIR_UP:    opposite = DIR_DOWN;  break;
    case DIR_DOWN:  opposite = DIR_UP;    break;
    case DIR_LEFT:  opposite = DIR_RIGHT; break;
    case DIR_RIGHT: opposite = DIR_LEFT;  break;
    }

    // 尝试3次找到一个非反方向
    for (int tries = 0; tries < 3; tries++) {
        Direction candidate = static_cast<Direction>(rand() % 4);
        if (candidate != opposite && candidate != blocked) {
            m_moveDir = candidate;
            return;
        }
    }
    // 回退：只要不是反方向就行
    m_moveDir = static_cast<Direction>((opposite + 1) % 4);
}

bool EnemyTank::isWallInFront(const QPointF& playerPos) const
{
    if (!scene()) return false;

    // 沿炮管方向逐格检查，最多检查 3 格
    int stepX = 0, stepY = 0;
    switch (m_direction) {
    case DIR_UP:    stepY = -1; break;
    case DIR_DOWN:  stepY =  1; break;
    case DIR_LEFT:  stepX = -1; break;
    case DIR_RIGHT: stepX =  1; break;
    }

    int startCol = static_cast<int>(x() / TILE_SIZE);
    int startRow = static_cast<int>(y() / TILE_SIZE);

    for (int i = 1; i <= 3; i++) {
        int checkCol = startCol + stepX * i;
        int checkRow = startRow + stepY * i;

        // 越界 → 安全
        if (checkRow < 0 || checkRow >= MAP_ROWS || checkCol < 0 || checkCol >= MAP_COLS)
            return false;

        // 检查这个格子是否有阻挡子弹的墙
        qreal cx = checkCol * TILE_SIZE + TILE_SIZE / 2.0;
        qreal cy = checkRow * TILE_SIZE + TILE_SIZE / 2.0;
        QRectF checkRect(cx - TILE_SIZE/2 + 2, cy - TILE_SIZE/2 + 2, TILE_SIZE - 4, TILE_SIZE - 4);
        QList<QGraphicsItem*> items = scene()->items(checkRect);
        for (auto* item : items) {
            Wall* wall = dynamic_cast<Wall*>(item);
            if (wall && wall->blocksBullet()) {
                // 玩家是否在墙之前？如果是，可以射击（玩家挡在中间）
                qreal wallDist = qSqrt((cx - x()) * (cx - x()) + (cy - y()) * (cy - y()));
                qreal playerDist = qSqrt(
                    (playerPos.x() - x()) * (playerPos.x() - x()) +
                    (playerPos.y() - y()) * (playerPos.y() - y())
                );
                if (playerDist < wallDist - TILE_SIZE * 0.5) {
                    continue;  // 玩家在墙前面，可以打玩家，忽略这堵墙
                }
                return true;  // 前方有墙且玩家不在中间 → 不安全
            }
        }
    }
    return false;  // 安全
}

void EnemyTank::changeRandomDirection()
{
    Direction old = m_moveDir;
    do {
        m_moveDir = static_cast<Direction>(rand() % 4);
    } while (m_moveDir == old); // 避免选到当前方向
}

Direction EnemyTank::directionTowardPlayer(const QPointF& playerPos) const
{
    qreal dx = playerPos.x() - x();
    qreal dy = playerPos.y() - y();
    // 选择与玩家偏差更大的轴向
    if (qAbs(dx) > qAbs(dy)) {
        return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    } else {
        return (dy > 0) ? DIR_DOWN : DIR_UP;
    }
}
