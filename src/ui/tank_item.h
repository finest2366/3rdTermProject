/**
 * @file tank_item.h
 * @brief 坦克独立图形项 —— 备用的独立坦克渲染方案
 *
 * 主渲染流程在 GameScene::syncWithEngine() 中。
 * TankItem 作为独立的 QGraphicsItem 子类保留，
 * 适用于需要单独动画或交互的坦克。
 */

#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include "../core/game_common.h"

class Tank;

class TankItem : public QGraphicsItem {
public:
    explicit TankItem(const Tank* tank, QGraphicsItem* parent = nullptr);
    ~TankItem() override = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void updateFromTank(const Tank* tank);

private:
    const Tank* m_tank = nullptr;
    double m_size = 36.0;
};
