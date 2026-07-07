#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "tank.h"
#include <QSet>

class PlayerTank : public Tank
{
public:
    // playerNum: 1 或 2 (决定颜色和键位)
    PlayerTank(qreal x, qreal y, int playerNum, QGraphicsItem* parent = nullptr);

    // QGraphicsItem 接口
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    // 处理输入 (检查按键状态)
    void processInput(const QSet<int>& keysPressed);

    // 属性
    int playerNum() const { return m_playerNum; }
    int score() const { return m_score; }
    void addScore(int pts = 1) { m_score += pts; }
    void resetScore() { m_score = 0; }

    // 键位绑定
    int keyUp() const { return m_keyUp; }
    int keyDown() const { return m_keyDown; }
    int keyLeft() const { return m_keyLeft; }
    int keyRight() const { return m_keyRight; }
    int keyShootUp() const    { return m_keyShootUp; }
    int keyShootDown() const  { return m_keyShootDown; }
    int keyShootLeft() const  { return m_keyShootLeft; }
    int keyShootRight() const { return m_keyShootRight; }

private:
    int m_playerNum;
    int m_score;
    int m_keyUp, m_keyDown, m_keyLeft, m_keyRight;           // 移动键
    int m_keyShootUp, m_keyShootDown, m_keyShootLeft, m_keyShootRight; // 射击方向键
};

#endif // PLAYERTANK_H
