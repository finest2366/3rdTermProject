#ifndef BASE_H
#define BASE_H

#include <QGraphicsItem>
#include <QPainter>
#include "constants.h"

class Base : public QGraphicsItem
{
public:
    Base(int row, int col, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QPainterPath shape() const override;

    bool isDestroyed() const { return m_destroyed; }
    void destroy();

private:
    int m_row, m_col;
    bool m_destroyed;
};

#endif // BASE_H
