/**
 * @file ai_controller.cpp
 * @brief AI 控制器基类实现
 */

#include "ai_controller.h"
#include "../core/enemy_tank.h"
#include "../core/bullet.h"
#include <QtMath>
#include <cmath>

double AIController::manhattanDist(const QPointF& a, const QPointF& b) {
    return qAbs(a.x() - b.x()) + qAbs(a.y() - b.y());
}

Direction AIController::directionTo(const QPointF& from, const QPointF& to) {
    double dx = to.x() - from.x();
    double dy = to.y() - from.y();
    if (qAbs(dx) > qAbs(dy)) {
        return dx > 0 ? Direction::Right : Direction::Left;
    } else {
        return dy > 0 ? Direction::Down : Direction::Up;
    }
}

Bullet* AIController::tryFire(EnemyTank* tank) {
    if (!tank || !tank->isAlive()) return nullptr;
    return tank->fire();  // Tank::canFire() 检查冷却
}

bool AIController::isTargetInLineOfFire(const EnemyTank* tank, const QPointF& targetPos,
                                         double fireRange, double alignThreshold) {
    if (!tank) return false;

    double tx = tank->position().x();
    double ty = tank->position().y();
    double px = targetPos.x();
    double py = targetPos.y();

    bool alignedH = qAbs(ty - py) < alignThreshold;
    bool alignedV = qAbs(tx - px) < alignThreshold;

    if (!alignedH && !alignedV) return false;

    Direction toTarget = directionTo(tank->position(), targetPos);
    if (toTarget != tank->direction()) return false;

    double dist = manhattanDist(tank->position(), targetPos);
    return dist < fireRange;
}
