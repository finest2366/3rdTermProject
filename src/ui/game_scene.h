/**
 * @file game_scene.h
 * @brief 游戏场景 —— QGraphicsScene 子类，将引擎实体同步为图形项
 *
 * v2.0 改进：
 * - 水域 (Water) 渲染
 * - 冰冻/隐身特效
 * - Boss 坦克更大尺寸和特殊颜色
 * - 道具名称缩写标签
 */

#pragma once

#include <QGraphicsScene>
#include <QVector>

class GameEngine;
class QGraphicsItem;

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject* parent = nullptr);
    ~GameScene() override;

    void setEngine(GameEngine* engine) { m_engine = engine; }

    /** @brief 每帧由 GameView::gameLoop() 调用 */
    void syncWithEngine();

private:
    void clearAllItems();
    QColor colorForTank(int ownerIndex) const;
    QColor colorForPowerUp(int typeInt) const;

    GameEngine* m_engine = nullptr;
    QVector<QGraphicsItem*> m_dynamicItems;
};
