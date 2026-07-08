/**
 * @file base.h
 * @brief 基地类 —— 被敌方子弹击中则游戏失败
 *
 * 双人模式中每个玩家各有一个基地。
 */

#pragma once

#include <QPointF>
#include <QSizeF>

class Base {
public:
    Base(const QPointF& pos, int cellSize, int playerIndex = 0);
    ~Base() = default;

    QPointF position() const { return m_position; }
    QSizeF size() const { return QSizeF(m_cellSize, m_cellSize); }
    bool isDestroyed() const { return m_destroyed; }
    void destroy() { m_destroyed = true; }
    int playerIndex() const { return m_playerIndex; }

private:
    QPointF m_position;
    int m_cellSize;
    int m_playerIndex;
    bool m_destroyed = false;
};
