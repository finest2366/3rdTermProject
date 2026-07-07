#include "wall.h"
#include <QGraphicsScene>

Wall::Wall(int row, int col, MapCell type, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_row(row), m_col(col), m_type(type)
    , m_hp(type == CELL_BRICK ? BRICK_HP : 999)
{
    setPos(col * TILE_SIZE + TILE_SIZE / 2.0,
           row * TILE_SIZE + TILE_SIZE / 2.0);

    // 草丛渲染在坦克上面，水和墙在下面
    if (type == CELL_GRASS)
        setZValue(3);
    else if (type == CELL_WATER)
        setZValue(0);
    else
        setZValue(1);
}

QRectF Wall::boundingRect() const
{
    qreal half = TILE_SIZE / 2.0;
    return QRectF(-half, -half, TILE_SIZE, TILE_SIZE);
}

void Wall::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QRectF rect = boundingRect();
    painter->setRenderHint(QPainter::Antialiasing);

    switch (m_type) {
    case CELL_BRICK: {
        // 砖墙 - 根据剩余HP改变颜色深度
        QColor brickColor = GameColors::BRICK;
        if (m_hp < BRICK_HP) {
            // 受损后颜色变暗
            int factor = 100 + (m_hp * 50); // HP=2→200, HP=1→150
            brickColor = brickColor.darker(factor);
        }
        painter->setBrush(brickColor);
        painter->setPen(QPen(brickColor.darker(150), 1));
        painter->drawRect(rect);

        // 画砖块纹理
        painter->setPen(QPen(GameColors::BRICK.darker(120), 1));
        qreal hw = TILE_SIZE / 2.0;
        qreal hh = TILE_SIZE / 2.0;
        painter->drawLine(QPointF(-hw, 0), QPointF(hw, 0));  // 水平中线
        painter->drawLine(QPointF(-hw/2, -hh), QPointF(-hw/2, 0)); // 上半左竖线
        painter->drawLine(QPointF(hw/2, 0), QPointF(hw/2, hh));    // 下半右竖线
        break;
    }
    case CELL_STEEL: {
        // 钢铁墙 - 灰色金属质感
        painter->setBrush(GameColors::STEEL);
        painter->setPen(QPen(GameColors::STEEL.darker(200), 2));
        painter->drawRect(rect);

        // 铆钉装饰
        painter->setBrush(GameColors::STEEL.lighter(150));
        painter->setPen(Qt::NoPen);
        qreal r = 3;
        qreal offset = TILE_SIZE / 2.0 - 6;
        painter->drawEllipse(QPointF(-offset, -offset), r, r);
        painter->drawEllipse(QPointF( offset, -offset), r, r);
        painter->drawEllipse(QPointF(-offset,  offset), r, r);
        painter->drawEllipse(QPointF( offset,  offset), r, r);
        break;
    }
    case CELL_WATER: {
        // 水域 - 蓝色带波纹
        painter->setBrush(GameColors::WATER);
        painter->setPen(QPen(GameColors::WATER.darker(130), 1));
        painter->drawRect(rect);

        // 波纹线条
        painter->setPen(QPen(GameColors::WATER.lighter(130), 1));
        qreal hh = TILE_SIZE / 2.0;
        painter->drawLine(QPointF(-hh + 4, -4), QPointF(hh - 4, -4));
        painter->drawLine(QPointF(-hh + 4,  4), QPointF(hh - 4,  4));
        break;
    }
    case CELL_GRASS: {
        // 草丛 - 绿色半透明
        painter->setBrush(GameColors::GRASS);
        painter->setPen(QPen(GameColors::GRASS.darker(120), 1));
        painter->drawRect(rect);

        // 草叶装饰
        painter->setPen(QPen(GameColors::GRASS.lighter(140), 1));
        qreal hh = TILE_SIZE / 2.0;
        painter->drawLine(QPointF(-8, -hh), QPointF(-4, hh));
        painter->drawLine(QPointF(0, -hh), QPointF(4, hh));
        painter->drawLine(QPointF(8, -hh), QPointF(12, hh));
        break;
    }
    default:
        break;
    }
}

void Wall::destroy()
{
    if (scene())
        scene()->removeItem(this);
    delete this;
}

bool Wall::takeDamage()
{
    if (m_type != CELL_BRICK) return false;
    m_hp--;
    update(); // 重绘显示受损状态
    if (m_hp <= 0) {
        destroy();
        return true; // 被摧毁
    }
    return false;   // 还活着
}
