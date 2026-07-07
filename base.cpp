#include "base.h"
#include <QGraphicsScene>
#include <QtMath>

Base::Base(int row, int col, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_row(row), m_col(col)
    , m_destroyed(false)
{
    setPos(col * TILE_SIZE + TILE_SIZE / 2.0,
           row * TILE_SIZE + TILE_SIZE / 2.0);
    setZValue(0); // 在坦克下面
}

QRectF Base::boundingRect() const
{
    qreal half = TILE_SIZE / 2.0;
    return QRectF(-half + 2, -half + 2, TILE_SIZE - 4, TILE_SIZE - 4);
}

QPainterPath Base::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Base::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_destroyed) return;

    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect = boundingRect();

    // 基地 - 金色旗帜标志
    painter->setBrush(GameColors::BASE);
    painter->setPen(QPen(GameColors::BASE.darker(150), 2));
    painter->drawRoundedRect(rect, 4, 4);

    // 画旗帜/星形标记
    painter->setBrush(GameColors::BASE.lighter(150));
    painter->setPen(Qt::NoPen);

    // 简化的旗帜形状
    QPainterPath flag;
    qreal cx = 0;
    qreal cy = 0;
    qreal s = TILE_SIZE / 2.0 - 6;

    // 五角星
    for (int i = 0; i < 5; i++) {
        qreal angle = -M_PI / 2 + i * 2 * M_PI / 5;
        qreal x = cx + s * cos(angle);
        qreal y = cy + s * sin(angle);
        if (i == 0) flag.moveTo(x, y);
        else flag.lineTo(x, y);

        angle += 2 * M_PI / 10;
        x = cx + s * 0.4 * cos(angle);
        y = cy + s * 0.4 * sin(angle);
        flag.lineTo(x, y);
    }
    flag.closeSubpath();
    painter->drawPath(flag);

    // 标签文字
    painter->setPen(Qt::white);
    QFont font;
    font.setPixelSize(10);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignBottom, "基地");
}

void Base::destroy()
{
    m_destroyed = true;
    update();
    // 不删除，让它变灰留在原地
}
