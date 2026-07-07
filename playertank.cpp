#include "playertank.h"
#include "bullet.h"
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QtMath>

PlayerTank::PlayerTank(qreal x, qreal y, int playerNum, QGraphicsItem* parent)
    : Tank(x, y, DIR_UP, TANK_HP, parent)
    , m_playerNum(playerNum)
    , m_score(0)
{
    // 设置键位：移动和射击分离
    if (playerNum == 1) {
        // Player 1: WASD 移动，方向键 射击
        m_keyUp    = Qt::Key_W;
        m_keyDown  = Qt::Key_S;
        m_keyLeft  = Qt::Key_A;
        m_keyRight = Qt::Key_D;
        m_keyShootUp    = Qt::Key_Up;
        m_keyShootDown  = Qt::Key_Down;
        m_keyShootLeft  = Qt::Key_Left;
        m_keyShootRight = Qt::Key_Right;
    } else {
        // Player 2: 方向键 移动，WASD 射击
        m_keyUp    = Qt::Key_Up;
        m_keyDown  = Qt::Key_Down;
        m_keyLeft  = Qt::Key_Left;
        m_keyRight = Qt::Key_Right;
        m_keyShootUp    = Qt::Key_W;
        m_keyShootDown  = Qt::Key_S;
        m_keyShootLeft  = Qt::Key_A;
        m_keyShootRight = Qt::Key_D;
    }
}

void PlayerTank::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // 选择颜色
    QColor bodyColor, barrelColor;
    if (m_playerNum == 1) {
        bodyColor = GameColors::PLAYER1_BODY;
        barrelColor = GameColors::PLAYER1_BARREL;
    } else {
        bodyColor = GameColors::PLAYER2_BODY;
        barrelColor = GameColors::PLAYER2_BARREL;
    }

    QRectF rect = boundingRect();

    // 绘制车身 (圆角矩形)
    painter->setBrush(bodyColor);
    painter->setPen(QPen(bodyColor.darker(150), 2));
    painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 6, 6);

    // 绘制履带装饰
    painter->setPen(QPen(bodyColor.darker(180), 3));
    qreal hw = TANK_SIZE / 2.0;
    qreal hh = TANK_SIZE / 2.0;
    if (m_direction == DIR_UP || m_direction == DIR_DOWN) {
        painter->drawLine(QPointF(-hw + 5, -hh), QPointF(-hw + 5, hh));
        painter->drawLine(QPointF( hw - 5, -hh), QPointF( hw - 5, hh));
    } else {
        painter->drawLine(QPointF(-hw, -hh + 5), QPointF(hw, -hh + 5));
        painter->drawLine(QPointF(-hw,  hh - 5), QPointF(hw,  hh - 5));
    }

    // 绘制炮管
    painter->setBrush(barrelColor);
    painter->setPen(QPen(barrelColor.darker(150), 1));
    qreal bw = 6;  // 炮管宽度
    qreal bl = TANK_SIZE / 2.0 + 4;  // 炮管长度
    QRectF barrelRect;
    switch (m_direction) {
    case DIR_UP:
        barrelRect = QRectF(-bw/2, -bl, bw, bl);
        break;
    case DIR_DOWN:
        barrelRect = QRectF(-bw/2, hh - 4, bw, bl);
        break;
    case DIR_LEFT:
        barrelRect = QRectF(-bl, -bw/2, bl, bw);
        break;
    case DIR_RIGHT:
        barrelRect = QRectF(hw - 4, -bw/2, bl, bw);
        break;
    }
    painter->drawRect(barrelRect);

    // 绘制炮塔 (中心圆形)
    painter->setBrush(bodyColor.lighter(130));
    painter->setPen(QPen(bodyColor.darker(120), 1));
    painter->drawEllipse(QPointF(0, 0), 8, 8);

    // 显示 HP 指示器 (小点)
    if (m_hp > 0) {
        painter->setBrush(Qt::white);
        painter->setPen(Qt::NoPen);
        for (int i = 0; i < m_hp; i++) {
            painter->drawEllipse(QPointF(-10 + i * 10, -hw + 10), 3, 3);
        }
    }
}

void PlayerTank::processInput(const QSet<int>& keysPressed)
{
    // 每帧重置惯性速度
    m_moveDx = 0;
    m_moveDy = 0;

    // === 移动：不改变朝向 ===
    if (keysPressed.contains(m_keyUp)) {
        tryMove(DIR_UP);
    }
    if (keysPressed.contains(m_keyDown)) {
        tryMove(DIR_DOWN);
    }
    if (keysPressed.contains(m_keyLeft)) {
        tryMove(DIR_LEFT);
    }
    if (keysPressed.contains(m_keyRight)) {
        tryMove(DIR_RIGHT);
    }

    // === 射击：朝向由射击方向决定 ===
    auto tryShoot = [&](Direction dir) {
        if (canShoot()) {
            m_direction = dir;  // 射击决定朝向
            Bullet* bullet = shoot();
            if (bullet && scene()) {
                scene()->addItem(bullet);
            }
        }
    };

    if (keysPressed.contains(m_keyShootUp))    tryShoot(DIR_UP);
    if (keysPressed.contains(m_keyShootDown))  tryShoot(DIR_DOWN);
    if (keysPressed.contains(m_keyShootLeft))  tryShoot(DIR_LEFT);
    if (keysPressed.contains(m_keyShootRight)) tryShoot(DIR_RIGHT);

    tickCooldown();
}
