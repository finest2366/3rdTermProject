/**
 * @file player_tank.h
 * @brief 玩家坦克 —— 响应键盘输入，支持双人不同键位
 *
 * 玩家1: W/A/S/D 移动, Space 射击, Q 技能
 * 玩家2: ↑/←/↓/→ 移动, Enter 射击, Shift 技能
 */

#pragma once

#include "tank.h"

class PlayerTank : public Tank {
public:
    explicit PlayerTank(int playerIndex, const QPointF& pos);
    ~PlayerTank() override = default;

    Bullet* fire() override;

    /** @brief 处理键盘事件，返回移动方向 */
    Direction handleKeyPress(int key) const;

    /** @brief 检查是否为开火键 */
    bool isFireKey(int key) const;

    /** @brief 检查是否为技能键 */
    bool isSkillKey(int key) const;

    /** @brief 获取该玩家的键位绑定 */
    int keyUp() const { return m_keyUp; }
    int keyDown() const { return m_keyDown; }
    int keyLeft() const { return m_keyLeft; }
    int keyRight() const { return m_keyRight; }

private:
    int m_keyUp, m_keyDown, m_keyLeft, m_keyRight;
    int m_keyFire;
    int m_keySkill;
};
