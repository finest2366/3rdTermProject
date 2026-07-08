/**
 * @file test_tank.cpp
 * @brief 单元测试 —— 覆盖核心游戏逻辑
 *
 * 测试范围：
 * - 坦克移动/伤害/治疗/护盾/冰冻/隐身
 * - 子弹飞行/失效/归属
 * - 墙壁类型/摧毁
 * - 道具效果 (7种)
 * - 技能系统 (冷却/激活)
 * - 碰撞检测
 * - AI 决策树节点
 * - 工厂注册与创建
 */

#include <gtest/gtest.h>
#include "../src/core/tank.h"
#include "../src/core/player_tank.h"
#include "../src/core/enemy_tank.h"
#include "../src/core/bullet.h"
#include "../src/core/wall.h"
#include "../src/core/base.h"
#include "../src/core/powerup.h"
#include "../src/core/skill.h"
#include "../src/core/game_common.h"
#include "../src/core/game_config.h"
#include "../src/core/map.h"
#include "../src/utils/factory.h"
#include "../src/ai/ai_controller.h"

// ============================================================
// 坦克基础测试
// ============================================================
TEST(TankTest, InitialState) {
    PlayerTank tank(0, QPointF(100, 100));
    EXPECT_TRUE(tank.isAlive());
    EXPECT_EQ(tank.hp(), GameConfig::PLAYER1_MAX_HP);
    EXPECT_EQ(tank.playerIndex(), 0);
    EXPECT_TRUE(tank.isPlayer());
}

TEST(TankTest, Movement) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.move(Direction::Right);
    EXPECT_GT(tank.position().x(), 100.0);
    tank.move(Direction::Down);
    EXPECT_GT(tank.position().y(), 100.0);
}

TEST(TankTest, TakeDamage) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.takeDamage(1);
    EXPECT_EQ(tank.hp(), 2);
    tank.takeDamage(2);
    EXPECT_EQ(tank.hp(), 0);
    EXPECT_FALSE(tank.isAlive());
}

TEST(TankTest, ShieldAbsorbsDamage) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.setShield(true);
    tank.takeDamage(1);
    EXPECT_EQ(tank.hp(), 3);       // 护盾吸收
    EXPECT_FALSE(tank.hasShield()); // 护盾消失
}

TEST(TankTest, HealingCapped) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.takeDamage(2);
    EXPECT_EQ(tank.hp(), 1);
    tank.heal(5);
    EXPECT_EQ(tank.hp(), 3);       // 不能超过最大HP
}

TEST(TankTest, FrozenCannotMove) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.setFrozen(true);
    QPointF before = tank.position();
    tank.move(Direction::Right);
    EXPECT_EQ(tank.position(), before); // 冰冻不能移动
}

TEST(TankTest, InvisibleFlag) {
    PlayerTank tank(0, QPointF(100, 100));
    EXPECT_FALSE(tank.isInvisible());
    tank.setInvisible(true);
    EXPECT_TRUE(tank.isInvisible());
}

TEST(TankTest, FireCooldown) {
    PlayerTank tank(0, QPointF(100, 100));
    EXPECT_TRUE(tank.canFire());
    Bullet* b = tank.fire();
    EXPECT_NE(b, nullptr);
    delete b;
    EXPECT_FALSE(tank.canFire()); // 冷却中
}

// ============================================================
// 子弹测试
// ============================================================
TEST(BulletTest, Movement) {
    Bullet bullet(QPointF(100, 100), Direction::Right, 0);
    bullet.update();
    EXPECT_GT(bullet.position().x(), 100.0);
}

TEST(BulletTest, Deactivation) {
    Bullet bullet(QPointF(100, 100), Direction::Up, 0);
    bullet.deactivate();
    EXPECT_FALSE(bullet.isActive());
    double x = bullet.position().x();
    bullet.update();
    EXPECT_EQ(bullet.position().x(), x); // 不活跃不移动
}

TEST(BulletTest, OwnerTracking) {
    Bullet pb(QPointF(0, 0), Direction::Up, 0);
    EXPECT_TRUE(pb.isPlayerBullet());
    Bullet eb(QPointF(0, 0), Direction::Down, -1);
    EXPECT_FALSE(eb.isPlayerBullet());
}

// ============================================================
// 墙壁测试
// ============================================================
TEST(WallTest, BrickDestructible) {
    Wall wall(QPointF(100, 100), WallType::Brick, 40);
    EXPECT_TRUE(wall.isDestructible());
    wall.destroy();
    EXPECT_FALSE(wall.isActive());
}

TEST(WallTest, SteelIndestructible) {
    Wall wall(QPointF(100, 100), WallType::Steel, 40);
    EXPECT_FALSE(wall.isDestructible());
    wall.destroy();
    EXPECT_TRUE(wall.isActive()); // 钢铁不毁
}

TEST(WallTest, WaterBlocksMovement) {
    Wall wall(QPointF(100, 100), WallType::Water, 40);
    EXPECT_TRUE(wall.blocksMovement());
    EXPECT_FALSE(wall.blocksBullets()); // 水域子弹可通过
}

// ============================================================
// 基地测试
// ============================================================
TEST(BaseTest, Destruction) {
    Base base(QPointF(280, 520), 40);
    EXPECT_FALSE(base.isDestroyed());
    base.destroy();
    EXPECT_TRUE(base.isDestroyed());
}

// ============================================================
// 道具测试 (7种)
// ============================================================
TEST(PowerUpTest, SpeedBoost) {
    PlayerTank tank(0, QPointF(100, 100));
    double orig = tank.speed();
    SpeedBoost pu;
    pu.apply(&tank);
    EXPECT_GT(tank.speed(), orig);
}

TEST(PowerUpTest, Shield) {
    PlayerTank tank(0, QPointF(100, 100));
    ShieldPowerUp pu;
    pu.apply(&tank);
    EXPECT_TRUE(tank.hasShield());
}

TEST(PowerUpTest, RapidFire) {
    PlayerTank tank(0, QPointF(100, 100));
    RapidFirePowerUp pu;
    pu.apply(&tank);
    EXPECT_TRUE(tank.hasRapidFire());
}

TEST(PowerUpTest, Heal) {
    PlayerTank tank(0, QPointF(100, 100));
    tank.takeDamage(2);
    HealPowerUp pu;
    pu.apply(&tank);
    EXPECT_EQ(tank.hp(), 2);
}

TEST(PowerUpTest, Bomb) {
    PlayerTank tank(0, QPointF(100, 100));
    BombPowerUp pu;
    pu.apply(&tank); // 炸弹对拾取者无直接效果
    EXPECT_TRUE(tank.isAlive());
}

TEST(PowerUpTest, Freeze) {
    PlayerTank tank(0, QPointF(100, 100));
    FreezePowerUp pu;
    pu.apply(&tank);
    // 冰冻效果在 GameEngine 中处理
    EXPECT_TRUE(tank.isAlive());
}

TEST(PowerUpTest, Invisible) {
    PlayerTank tank(0, QPointF(100, 100));
    InvisiblePowerUp pu;
    pu.apply(&tank);
    EXPECT_TRUE(tank.isInvisible());
}

// ============================================================
// 技能测试
// ============================================================
TEST(SkillTest, SprintActivates) {
    PlayerTank tank(0, QPointF(100, 100));
    auto skill = std::make_unique<SprintSkill>();
    EXPECT_TRUE(skill->isReady());
    GameState state;
    bool ok = skill->activate(&tank, state);
    EXPECT_TRUE(ok);
    EXPECT_FALSE(skill->isReady()); // 进入冷却
}

TEST(SkillTest, CooldownProgress) {
    PlayerTank tank(0, QPointF(100, 100));
    auto skill = std::make_unique<SprintSkill>();
    EXPECT_FLOAT_EQ(skill->cooldownPercent(), 1.0f);
    skill->activate(&tank, GameState{});
    EXPECT_LT(skill->cooldownPercent(), 1.0f);
}

// ============================================================
// 碰撞检测测试
// ============================================================
TEST(CollisionTest, RectIntersect) {
    QRectF r1(100, 100, 40, 40);
    QRectF r2(120, 120, 40, 40);
    EXPECT_TRUE(r1.intersects(r2));
    QRectF r3(200, 200, 40, 40);
    EXPECT_FALSE(r1.intersects(r3));
}

// ============================================================
// 工厂测试
// ============================================================
TEST(FactoryTest, PowerUpFactoryCreates) {
    PowerUp* pu = PowerUpFactory::instance().create("SpeedBoost");
    EXPECT_NE(pu, nullptr);
    delete pu;
}

TEST(FactoryTest, AIFactoryCreates) {
    AIController* ai = AIControllerFactory::instance().create("PatrolAI");
    EXPECT_NE(ai, nullptr);
    delete ai;
}

TEST(FactoryTest, UnknownReturnsNull) {
    EXPECT_EQ(PowerUpFactory::instance().create("NonExistent"), nullptr);
    EXPECT_EQ(AIControllerFactory::instance().create("NonExistent"), nullptr);
}

// ============================================================
// AI 测试
// ============================================================
TEST(AITest, ManhattanDist) {
    QPointF a(0, 0), b(3, 4);
    // 曼哈顿距离 = |3-0| + |4-0| = 7
    EXPECT_DOUBLE_EQ(AIController::manhattanDist(a, b), 7.0);
}

TEST(AITest, DirectionTo) {
    EXPECT_EQ(AIController::directionTo(QPointF(0, 0), QPointF(0, -10)), Direction::Up);
    EXPECT_EQ(AIController::directionTo(QPointF(0, 0), QPointF(10, 0)), Direction::Right);
    EXPECT_EQ(AIController::directionTo(QPointF(0, 0), QPointF(0, 10)), Direction::Down);
    EXPECT_EQ(AIController::directionTo(QPointF(0, 0), QPointF(-10, 0)), Direction::Left);
}

// ============================================================
// 主函数
// ============================================================
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
