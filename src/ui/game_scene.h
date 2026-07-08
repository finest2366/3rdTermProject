/**
 * @file game_scene.h
 * @brief 游戏场景渲染器 —— 使用 QPainter 直接绘制，零分配开销
 *
 * v2.1 改进：
 * - 从 QGraphicsScene 重构为纯 QPainter 渲染
 * - 每帧零堆分配，消除 QGraphicsItem 创建/销毁开销
 */

#pragma once

#include <QObject>
#include <QColor>

class QPainter;
class GameEngine;

class GameScene : public QObject {
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);
    ~GameScene() override;

    void setEngine(GameEngine* engine) { m_engine = engine; }

    /** @brief 使用 QPainter 直接渲染所有游戏实体 */
    void render(QPainter& painter, GameEngine* engine);

private:
    void drawWalls(QPainter& painter, GameEngine* engine);
    void drawPowerUps(QPainter& painter, GameEngine* engine);
    void drawTanks(QPainter& painter, GameEngine* engine);
    void drawBullets(QPainter& painter, GameEngine* engine);
    void drawBases(QPainter& painter, GameEngine* engine);

    QColor colorForTank(int ownerIndex) const;
    QColor colorForPowerUp(int typeInt) const;

    GameEngine* m_engine = nullptr;
};
