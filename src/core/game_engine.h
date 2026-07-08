/**
 * @file game_engine.h
 * @brief 游戏主引擎 —— 管理游戏主循环、碰撞检测、积分、胜负判定
 *
 * 通过 QTimer 驱动 update() 方法（约 60 FPS）。
 * 与 UI 层解耦，可独立进行单元测试。
 *
 * 新增功能（v2.0）：
 * - AI 自主决定射击（非全局随机概率）
 * - 技能激活与冷却管理
 * - 冰冻/隐身道具效果
 * - Boss 敌人支持
 * - Water 类型墙壁（阻挡移动但不阻挡子弹）
 * - 导弹（穿透墙壁的子弹）
 * - 脏标记优化渲染
 */

#pragma once

#include "game_common.h"
#include "game_config.h"
#include <QObject>
#include <QPointF>
#include <QSizeF>
#include <QVector>
#include <memory>
#include <vector>

class Tank;
class Bullet;
class Wall;
class Base;
class PowerUp;
class AIController;

class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject* parent = nullptr);
    ~GameEngine() override;

    // ---- 游戏启动 ----
    void startSingleGame(int levelId);
    void startMultiGame();

    // ---- 每帧更新 ----
    void update(int deltaMs);

    // ---- 玩家输入 ----
    void handleInput(int playerIndex, int direction, bool fire);

    /** @brief 激活指定玩家的技能 */
    bool activateSkill(int playerIndex);

    // ---- 地图构建（供 Map 类调用） ----
    void addWall(const Wall& wall);

    // ---- 只读访问 ----
    const std::vector<std::unique_ptr<Tank>>& tanks() const { return m_tanks; }
    const std::vector<std::unique_ptr<Bullet>>& bullets() const { return m_bullets; }
    const std::vector<std::unique_ptr<Wall>>& walls() const { return m_walls; }
    const std::vector<std::unique_ptr<PowerUp>>& powerUps() const { return m_powerUps; }
    Base* base() const { return m_base.get(); }
    Base* base2() const { return m_base2.get(); }

    int playerLives(int index) const;
    int playerScore(int index) const;
    int remainingEnemies() const { return m_remainingEnemies; }
    int totalEnemies() const { return m_totalEnemies; }
    bool isGameOver() const { return m_gameOver; }
    int winner() const { return m_winner; }
    bool isMultiPlayer() const { return m_isMultiPlayer; }
    int currentLevel() const { return m_currentLevel; }

    /** @brief 获取当前游戏状态快照（供 AI 使用） */
    GameState getGameState() const;

    /** @brief 渲染脏标记（供 UI 层判断是否需要重绘） */
    bool isDirty() const { return m_dirty; }
    void clearDirty() { m_dirty = false; }

    /** @brief 获取自游戏开始以来的累计帧时间（毫秒），用于动画效果 */
    int lastFrameMs() const { return m_lastFrameMs; }

signals:
    void gameOver(int winner);      // 0=失败, 1=P1胜, 2=P2胜
    void scoreChanged(int playerIndex, int newScore);
    void livesChanged(int playerIndex, int newLives);
    void skillActivated(int playerIndex, const QString& skillName);
    void powerUpCollected(int playerIndex, const QString& powerUpName);

private:
    // ---- 关卡加载 ----
    void loadLevel(int levelId);

    // ---- 实体生成 ----
    void spawnPlayer(int playerIndex, const QPointF& pos);
    void spawnEnemy(TankType type, const QPointF& pos, const QString& aiType);
    void spawnPowerUps(const QStringList& powerUpNames);

    // ---- 每帧子步骤 ----
    void updateEnemyAI();
    void updateBullets();
    void updateTimedEffects(int frameDeltaMs);
    void checkCollisions();
    void checkWinCondition();
    void removeDeadEntities();

    // ---- 碰撞检测 ----
    bool checkWallCollision(const QPointF& pos, const QSizeF& size) const;
    bool checkBulletWallCollision(const QPointF& pos, const QSizeF& size) const;
    bool checkTankCollision(const QPointF& pos, const QSizeF& size, int excludeIndex) const;
    QPointF clampToMap(const QPointF& pos, const QSizeF& size) const;

    // ---- 子弹特殊类型 ----
    /** @brief 发射一枚可穿透墙壁的导弹 */
    void spawnMissile(const Tank* tank);

    // 地图尺寸
    static constexpr int MAP_COLS = GameConfig::MAP_COLS;
    static constexpr int MAP_ROWS = GameConfig::MAP_ROWS;
    static constexpr int CELL_SIZE = GameConfig::CELL_SIZE;
    int mapWidth() const { return MAP_COLS * CELL_SIZE; }
    int mapHeight() const { return MAP_ROWS * CELL_SIZE; }

    // ---- 实体容器 ----
    std::vector<std::unique_ptr<Tank>> m_tanks;
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    std::vector<std::unique_ptr<Wall>> m_walls;
    std::vector<std::unique_ptr<PowerUp>> m_powerUps;
    std::unique_ptr<Base> m_base;
    std::unique_ptr<Base> m_base2;  // 双人模式第二个基地

    // ---- 玩家状态 ----
    struct PlayerState {
        int lives = GameConfig::PLAYER_SINGLE_LIVES;
        int score = 0;
        int sprintTimer = 0;        // 疾跑剩余时间（毫秒）
    };
    PlayerState m_playerStates[2];

    // ---- 全局效果计时器 ----
    int m_freezeTimer = 0;          // 冰冻剩余时间

    // ---- 游戏状态 ----
    bool m_isMultiPlayer = false;
    bool m_gameOver = false;
    int m_winner = 0;
    int m_currentLevel = 1;
    int m_remainingEnemies = 0;
    int m_totalEnemies = 0;
    bool m_dirty = true;            // 渲染脏标记

    // ---- 帧计时 ----
    int m_lastFrameMs = 0;
};
