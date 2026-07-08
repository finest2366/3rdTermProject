/**
 * @file wall.cpp
 * @brief 墙壁实现
 */

#include "wall.h"

Wall::Wall(const QPointF& pos, WallType type, int cellSize)
    : m_position(pos)
    , m_type(type)
    , m_cellSize(cellSize)
{
}
