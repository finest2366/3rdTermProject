#ifndef WALL_H
#define WALL_H

#include <QGraphicsItem>
#include <QPainter>
#include "constants.h"

class Wall : public QGraphicsItem
{
public:
    Wall(int row, int col, MapCell type, QGraphicsItem* parent = nullptr);

    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    // 属性
    MapCell cellType() const { return m_type; }
    bool isDestructible() const { return m_type == CELL_BRICK; }
    bool blocksTank() const { return m_type == CELL_BRICK || m_type == CELL_STEEL || m_type == CELL_WATER; }
    bool blocksBullet() const { return m_type == CELL_BRICK || m_type == CELL_STEEL; }
    bool isWater() const { return m_type == CELL_WATER; }
    bool isGrass() const { return m_type == CELL_GRASS; }
    int gridRow() const { return m_row; }
    int gridCol() const { return m_col; }

    void destroy();           // 立即摧毁
    bool takeDamage();        // 受到一次攻击，返回true表示被摧毁
    int hp() const { return m_hp; }

private:
    int m_row, m_col;
    MapCell m_type;
    int m_hp;                 // 当前生命值（仅砖墙有效）
};

#endif // WALL_H
