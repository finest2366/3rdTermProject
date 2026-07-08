/**
 * @file wall.h
 * @brief 墙壁类 —— 支持砖墙、钢铁墙和水域三种类型
 *
 * - Brick:  可被子弹摧毁
 * - Steel:  不可摧毁
 * - Water:  不可穿越，但子弹可以通过
 */

#pragma once

#include "game_common.h"
#include <QPointF>
#include <QSizeF>

class Wall {
public:
    Wall(const QPointF& pos, WallType type, int cellSize);
    ~Wall() = default;

    QPointF position() const { return m_position; }
    QSizeF size() const { return QSizeF(m_cellSize, m_cellSize); }
    WallType wallType() const { return m_type; }

    bool isDestructible() const { return m_type == WallType::Brick; }
    bool blocksBullets() const { return m_type != WallType::Water; }
    bool blocksMovement() const { return true; }  // 所有墙壁都阻挡移动

    bool isActive() const { return m_active; }
    void destroy() { if (isDestructible()) m_active = false; }

private:
    QPointF m_position;
    WallType m_type;
    int m_cellSize;
    bool m_active = true;
};
