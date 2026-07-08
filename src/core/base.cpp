/**
 * @file base.cpp
 * @brief 基地实现
 */

#include "base.h"

Base::Base(const QPointF& pos, int cellSize, int playerIndex)
    : m_position(pos)
    , m_cellSize(cellSize)
    , m_playerIndex(playerIndex)
{
}
