/**
 * @file tank_item.cpp
 * @brief 坦克独立图形项实现
 */

#include "tank_item.h"
#include "../core/tank.h"
#include <QPainter>
#include <QtMath>

TankItem::TankItem(const Tank* tank, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_tank(tank)
{
    if (tank) {
        m_size = tank->sizeValue();
        setPos(tank->position());
    }
}

QRectF TankItem::boundingRect() const {
    double margin = 4.0;
    return QRectF(-m_size / 2 - margin, -m_size / 2 - margin,
                   m_size + 2 * margin, m_size + 2 * margin);
}

void TankItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!m_tank) return;

    painter->setRenderHint(QPainter::Antialiasing);

    // 坦克主体
    QRectF bodyRect(-m_size / 2, -m_size / 2, m_size, m_size);
    QColor bodyColor;
    if (m_tank->isPlayer()) {
        bodyColor = (m_tank->playerIndex() == 0) ? QColor(0, 200, 0) : QColor(0, 100, 255);
    } else {
        bodyColor = QColor(200, 50, 50);
    }
    // 隐身
    if (m_tank->isInvisible()) bodyColor.setAlphaF(0.3);

    painter->setBrush(bodyColor);
    painter->setPen(QPen(Qt::white, 2));
    painter->drawRect(bodyRect);

    // 炮管
    painter->setPen(QPen(Qt::white, 3));
    double barrelLen = m_size / 2 + 4;
    QPointF barrelEnd;
    switch (m_tank->direction()) {
    case Direction::Up:    barrelEnd = QPointF(0, -barrelLen); break;
    case Direction::Down:  barrelEnd = QPointF(0, barrelLen); break;
    case Direction::Left:  barrelEnd = QPointF(-barrelLen, 0); break;
    case Direction::Right: barrelEnd = QPointF(barrelLen, 0); break;
    }
    painter->drawLine(QPointF(0, 0), barrelEnd);

    // 护盾
    if (m_tank->hasShield()) {
        painter->setPen(QPen(QColor(0, 128, 255), 2));
        painter->setBrush(QColor(0, 128, 255, 40));
        painter->drawEllipse(QPointF(0, 0), m_size / 2 + 3, m_size / 2 + 3);
    }
}

void TankItem::updateFromTank(const Tank* tank) {
    m_tank = tank;
    if (tank) {
        setPos(tank->position());
    }
    update();
}
