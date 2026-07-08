/**
 * @file game_scene.cpp
 * @brief 游戏场景渲染实现 —— QPainter 直接绘制
 *
 * 渲染顺序：墙壁 → 道具 → 坦克 → 子弹 → 基地
 * v2.1：从 QGraphicsItem 重构为 QPainter，零每帧分配
 */

#include "game_scene.h"
#include "../core/game_engine.h"
#include "../core/tank.h"
#include "../core/bullet.h"
#include "../core/wall.h"
#include "../core/base.h"
#include "../core/powerup.h"
#include "../core/game_common.h"

#include <QPainter>
#include <QFont>

GameScene::GameScene(QObject* parent)
    : QObject(parent)
{
}

GameScene::~GameScene() = default;

void GameScene::render(QPainter& painter, GameEngine* engine) {
    if (!engine) return;

    drawWalls(painter, engine);
    drawPowerUps(painter, engine);
    drawTanks(painter, engine);
    drawBullets(painter, engine);
    drawBases(painter, engine);
}

void GameScene::drawWalls(QPainter& painter, GameEngine* engine) {
    for (const auto& wall : engine->walls()) {
        if (!wall->isActive()) continue;

        QColor fillColor;
        switch (wall->wallType()) {
        case WallType::Brick: fillColor = QColor(139, 90, 43); break;
        case WallType::Steel: fillColor = QColor(128, 128, 128); break;
        case WallType::Water: fillColor = QColor(30, 100, 200); break;
        }

        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(QBrush(fillColor));
        painter.drawRect(QRectF(wall->position(), wall->size()));

        // 水域波纹效果
        if (wall->wallType() == WallType::Water) {
            double y = wall->position().y() + wall->size().height() / 2;
            painter.setPen(QPen(QColor(100, 180, 255, 100), 1));
            painter.drawLine(
                QPointF(wall->position().x() + 2, y),
                QPointF(wall->position().x() + wall->size().width() - 2, y));
        }
    }
}

void GameScene::drawPowerUps(QPainter& painter, GameEngine* engine) {
    for (const auto& pu : engine->powerUps()) {
        if (!pu->isActive()) continue;

        QColor puColor = colorForPowerUp(static_cast<int>(pu->powerUpType()));

        painter.setPen(QPen(Qt::white, 1));
        painter.setBrush(QBrush(puColor));
        painter.drawRect(QRectF(pu->position(), pu->size()));

        // 道具首字母标签
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(
            QRectF(pu->position().x() + 2, pu->position().y() + 2,
                   pu->size().width(), pu->size().height()),
            Qt::AlignCenter, pu->name().left(1));
    }
}

void GameScene::drawTanks(QPainter& painter, GameEngine* engine) {
    for (const auto& tank : engine->tanks()) {
        if (!tank->isAlive()) continue;

        // 隐身特效：半透明
        QColor color = colorForTank(tank->playerIndex());
        double alpha = tank->isInvisible() ? 0.3 : 1.0;
        color.setAlphaF(alpha);

        // 冰冻特效：蓝色调
        if (tank->isFrozen() && !tank->isPlayer()) {
            color = QColor(100, 150, 255);
        }

        double size = tank->sizeValue();

        // 坦克主体
        painter.setPen(QPen(tank->isFrozen() ? QColor(0, 200, 255) : Qt::white,
                            tank->type() == TankType::EnemyBoss ? 3 : 2));
        painter.setBrush(QBrush(color));
        painter.drawRect(QRectF(tank->position(), QSizeF(size, size)));

        // 血条
        {
            double barWidth = size;
            double barHeight = 5.0;
            double barX = tank->position().x();
            double barY = tank->position().y() - barHeight - 2.0;

            // 背景
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(40, 40, 40)));
            painter.drawRect(QRectF(barX, barY, barWidth, barHeight));

            // 前景
            double hpRatio = static_cast<double>(tank->hp()) / tank->maxHp();
            QColor hpColor = (hpRatio > 0.5) ? QColor(0, 255, 0)
                           : (hpRatio > 0.25) ? QColor(255, 255, 0)
                           : QColor(255, 0, 0);
            painter.setBrush(QBrush(hpColor));
            painter.drawRect(QRectF(barX + 1, barY + 1,
                                    (barWidth - 2) * hpRatio, barHeight - 2));
        }

        // 炮管
        double cx = tank->position().x() + size / 2;
        double cy = tank->position().y() + size / 2;
        double barrelLen = size / 2 + 4;
        double bx = cx, by = cy;

        switch (tank->direction()) {
        case Direction::Up:    by -= barrelLen; break;
        case Direction::Down:  by += barrelLen; break;
        case Direction::Left:  bx -= barrelLen; break;
        case Direction::Right: bx += barrelLen; break;
        }

        painter.setPen(QPen(Qt::white, tank->type() == TankType::EnemyBoss ? 4 : 3));
        painter.drawLine(QPointF(cx, cy), QPointF(bx, by));

        // 护盾效果
        if (tank->hasShield()) {
            painter.setPen(QPen(QColor(0, 180, 255, 200), 2));
            painter.setBrush(QBrush(QColor(0, 128, 255, 50)));
            painter.drawEllipse(QRectF(
                tank->position().x() - 3, tank->position().y() - 3,
                size + 6, size + 6));
        }

        // Boss 标记
        if (tank->type() == TankType::EnemyBoss) {
            painter.setPen(QColor(255, 50, 50));
            painter.setFont(QFont("Arial", 9, QFont::Bold));
            painter.drawText(
                QPointF(tank->position().x() + 2, tank->position().y() - 18),
                "BOSS");
        }
    }
}

void GameScene::drawBullets(QPainter& painter, GameEngine* engine) {
    painter.setPen(Qt::NoPen);

    for (const auto& bullet : engine->bullets()) {
        if (!bullet->isActive()) continue;

        QColor bulletColor = bullet->isPlayerBullet()
            ? QColor(255, 255, 100)   // 玩家：黄色
            : QColor(255, 100, 100);   // 敌方：红色

        painter.setBrush(QBrush(bulletColor));
        painter.drawEllipse(QRectF(bullet->position(), bullet->size()));
    }
}

void GameScene::drawBases(QPainter& painter, GameEngine* engine) {
    auto drawBase = [&](Base* base) {
        if (!base || base->isDestroyed()) return;

        painter.setPen(QPen(Qt::yellow, 2));
        painter.setBrush(QBrush(QColor(50, 50, 50)));
        painter.drawRect(QRectF(base->position(), base->size()));

        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(QRectF(base->position(), base->size()),
                         Qt::AlignCenter, "HQ");
    };
    drawBase(engine->base());
    drawBase(engine->base2());
}

QColor GameScene::colorForTank(int ownerIndex) const {
    switch (ownerIndex) {
    case 0:  return QColor(0, 200, 0);     // 玩家1：绿色
    case 1:  return QColor(0, 100, 255);   // 玩家2：蓝色
    default: return QColor(200, 50, 50);   // 敌方：红色
    }
}

QColor GameScene::colorForPowerUp(int typeInt) const {
    switch (static_cast<PowerUpType>(typeInt)) {
    case PowerUpType::Speed:     return QColor(0, 255, 0);     // 绿色
    case PowerUpType::Shield:    return QColor(0, 128, 255);   // 蓝色
    case PowerUpType::RapidFire: return QColor(255, 128, 0);   // 橙色
    case PowerUpType::Heal:      return QColor(255, 0, 0);     // 红色
    case PowerUpType::Bomb:      return QColor(255, 255, 0);   // 黄色
    case PowerUpType::Freeze:    return QColor(0, 255, 255);   // 青色
    case PowerUpType::Invisible: return QColor(180, 180, 180); // 灰色
    default: return QColor(200, 200, 200);
    }
}
