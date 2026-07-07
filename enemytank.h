#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "tank.h"
#include <QTimer>

class EnemyTank : public Tank
{
public:
    enum Difficulty { EASY, NORMAL, HARD };

    EnemyTank(qreal x, qreal y, Difficulty diff = NORMAL, QGraphicsItem* parent = nullptr);

    // QGraphicsItem 接口
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    // AI 更新 (每帧由 GameScene 调用)
    void updateAI(const QPointF& playerPos);

    // 当撞墙时改变方向
    void onBlocked();

private:
    void changeRandomDirection();
    Direction directionTowardPlayer(const QPointF& playerPos) const;
    bool isWallInFront(const QPointF& playerPos) const;  // 前方有墙且玩家不在中间 → true
    bool tryDodgeBullets();             // 检测并躲避飞来的子弹，返回是否在闪避

    Difficulty m_difficulty;
    Direction m_moveDir;         // 移动方向（独立于朝向）
    int m_directionTimer;        // 改变移动方向的计时器
    int m_shootDecisionTimer;    // 射击决策计时器

    // AI 参数
    int m_dirChangeInterval;     // 改变方向间隔
    int m_shootInterval;         // 射击间隔
    qreal m_speed;               // 移动速度
};

#endif // ENEMYTANK_H
