/**
 * @file game_scene.cpp
 * @brief 游戏场景渲染实现
 *
 * 渲染顺序：背景 → 墙壁 → 道具 → 坦克 → 子弹 → 基地
 * v2.0 新增：水域/冰冻/隐身/Boss特效渲染
 */

#include "game_scene.h"
#include "../core/game_engine.h"
#include "../core/tank.h"
#include "../core/bullet.h"
#include "../core/wall.h"
#include "../core/base.h"
#include "../core/powerup.h"
#include "../core/game_common.h"

#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>

GameScene::GameScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(QBrush(QColor(20, 20, 30)));  // 深蓝灰背景
}

GameScene::~GameScene() = default;

void GameScene::syncWithEngine() {
    if (!m_engine) return;

    // 清除上一帧
    clearAllItems();

    // ===== 1. 墙壁 =====
    for (const auto& wall : m_engine->walls()) {
        if (!wall->isActive()) continue;

        QColor fillColor;
        switch (wall->wallType()) {
        case WallType::Brick:
            fillColor = QColor(139, 90, 43);   // 棕色砖墙
            break;
        case WallType::Steel:
            fillColor = QColor(128, 128, 128);  // 灰色钢铁
            break;
        case WallType::Water:
            fillColor = QColor(30, 100, 200);   // 蓝色水域
            break;
        }

        auto* item = addRect(
            wall->position().x(), wall->position().y(),
            wall->size().width(), wall->size().height(),
            QPen(Qt::black, 1),
            QBrush(fillColor)
        );
        m_dynamicItems.append(item);

        // 水域加波纹效果（水平线）
        if (wall->wallType() == WallType::Water) {
            double y = wall->position().y() + wall->size().height() / 2;
            auto* waveLine = addLine(
                wall->position().x() + 2, y,
                wall->position().x() + wall->size().width() - 2, y,
                QPen(QColor(100, 180, 255, 100), 1)
            );
            m_dynamicItems.append(waveLine);
        }
    }

    // ===== 2. 道具 =====
    for (const auto& pu : m_engine->powerUps()) {
        if (!pu->isActive()) continue;

        QColor puColor = colorForPowerUp(static_cast<int>(pu->powerUpType()));

        auto* rectItem = addRect(
            pu->position().x(), pu->position().y(),
            pu->size().width(), pu->size().height(),
            QPen(Qt::white, 1),
            QBrush(puColor)
        );
        m_dynamicItems.append(rectItem);

        // 道具首字母标签
        auto* textItem = addSimpleText(pu->name().left(1), QFont("Arial", 10, QFont::Bold));
        textItem->setPos(pu->position().x() + 8, pu->position().y() + 4);
        textItem->setBrush(Qt::white);
        m_dynamicItems.append(textItem);
    }

    // ===== 3. 坦克 =====
    for (const auto& tank : m_engine->tanks()) {
        if (!tank->isAlive()) continue;

        // 隐身特效：半透明
        QColor color = colorForTank(tank->playerIndex());
        double alpha = tank->isInvisible() ? 0.3 : 1.0;
        color.setAlphaF(alpha);

        // 冰冻特效：蓝色调
        if (tank->isFrozen() && !tank->isPlayer()) {
            color = QColor(100, 150, 255);
        }

        // Boss 更大
        double size = tank->sizeValue();

        // 坦克主体
        auto* bodyItem = addRect(
            tank->position().x(), tank->position().y(),
            size, size,
            QPen(tank->isFrozen() ? QColor(0, 200, 255) : Qt::white, tank->type() == TankType::EnemyBoss ? 3 : 2),
            QBrush(color)
        );
        m_dynamicItems.append(bodyItem);

        // 血条
        {
            double barWidth = size;
            double barHeight = 5.0;
            double barX = tank->position().x();
            double barY = tank->position().y() - barHeight - 2.0;

            // 背景
            auto* barBg = addRect(barX, barY, barWidth, barHeight,
                                  QPen(Qt::NoPen), QBrush(QColor(40, 40, 40)));
            m_dynamicItems.append(barBg);

            // 前景
            double hpRatio = static_cast<double>(tank->hp()) / tank->maxHp();
            QColor hpColor = (hpRatio > 0.5) ? QColor(0, 255, 0)
                           : (hpRatio > 0.25) ? QColor(255, 255, 0)
                           : QColor(255, 0, 0);
            auto* barFg = addRect(barX + 1, barY + 1,
                                  (barWidth - 2) * hpRatio, barHeight - 2,
                                  QPen(Qt::NoPen), QBrush(hpColor));
            m_dynamicItems.append(barFg);
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

        QPen barrelPen(Qt::white, tank->type() == TankType::EnemyBoss ? 4 : 3);
        auto* barrelItem = addLine(cx, cy, bx, by, barrelPen);
        m_dynamicItems.append(barrelItem);

        // 护盾效果
        if (tank->hasShield()) {
            auto* shieldItem = addEllipse(
                tank->position().x() - 3, tank->position().y() - 3,
                size + 6, size + 6,
                QPen(QColor(0, 180, 255, 200), 2),
                QBrush(QColor(0, 128, 255, 50))
            );
            m_dynamicItems.append(shieldItem);
        }

        // Boss 特殊标记
        if (tank->type() == TankType::EnemyBoss) {
            auto* bossText = addSimpleText("BOSS", QFont("Arial", 9, QFont::Bold));
            bossText->setPos(tank->position().x() + 2, tank->position().y() - 18);
            bossText->setBrush(QColor(255, 50, 50));
            m_dynamicItems.append(bossText);
        }
    }

    // ===== 4. 子弹 =====
    for (const auto& bullet : m_engine->bullets()) {
        if (!bullet->isActive()) continue;

        QColor bulletColor = bullet->isPlayerBullet()
            ? QColor(255, 255, 100)   // 玩家：黄色
            : QColor(255, 100, 100);   // 敌方：红色

        auto* bulletItem = addEllipse(
            bullet->position().x(), bullet->position().y(),
            bullet->size().width(), bullet->size().height(),
            QPen(Qt::NoPen),
            QBrush(bulletColor)
        );
        m_dynamicItems.append(bulletItem);
    }

    // ===== 5. 基地 =====
    auto drawBase = [this](Base* base) {
        if (!base || base->isDestroyed()) return;
        auto* baseRect = addRect(
            base->position().x(), base->position().y(),
            base->size().width(), base->size().height(),
            QPen(Qt::yellow, 2),
            QBrush(QColor(50, 50, 50))
        );
        m_dynamicItems.append(baseRect);

        auto* hqText = addSimpleText("HQ", QFont("Arial", 12, QFont::Bold));
        hqText->setPos(base->position().x() + 6, base->position().y() + 8);
        hqText->setBrush(Qt::yellow);
        m_dynamicItems.append(hqText);
    };
    drawBase(m_engine->base());
    drawBase(m_engine->base2());
}

void GameScene::clearAllItems() {
    QList<QGraphicsItem*> allItems = items();
    for (auto* item : allItems) {
        removeItem(item);
        delete item;
    }
    m_dynamicItems.clear();
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
