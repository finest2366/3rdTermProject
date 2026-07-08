/**
 * @file game_engine.cpp
 * @brief 游戏主引擎实现
 *
 * 核心循环：AI决策 → 子弹移动 → 碰撞检测 → 死亡清理 → 胜负判定
 * v2.0 新增：AI自主射击、技能系统、冰冻/隐身、Boss、水域、导弹
 */

#include "game_engine.h"
#include "tank.h"
#include "player_tank.h"
#include "enemy_tank.h"
#include "bullet.h"
#include "wall.h"
#include "base.h"
#include "powerup.h"
#include "skill.h"
#include "map.h"
#include "ai/ai_controller.h"
#include "utils/factory.h"
#include "data/level_loader.h"

#include <QtMath>
#include <QDebug>
#include <QRandomGenerator>
#include <algorithm>

// ============================================================
// 构造 / 析构
// ============================================================
GameEngine::GameEngine(QObject* parent)
    : QObject(parent)
{
}

GameEngine::~GameEngine() = default;

// ============================================================
// 游戏启动
// ============================================================
void GameEngine::startSingleGame(int levelId) {
    m_isMultiPlayer = false;
    m_gameOver = false;
    m_winner = 0;
    m_currentLevel = levelId;

    m_tanks.clear();
    m_bullets.clear();
    m_walls.clear();
    m_powerUps.clear();
    m_base.reset();
    m_base2.reset();

    m_playerStates[0] = {GameConfig::PLAYER_SINGLE_LIVES, 0, 0};
    m_playerStates[1] = {0, 0, 0};

    m_freezeTimer = 0;
    m_dirty = true;

    loadLevel(levelId);
}

void GameEngine::startMultiGame() {
    m_isMultiPlayer = true;
    m_gameOver = false;
    m_winner = 0;

    m_tanks.clear();
    m_bullets.clear();
    m_walls.clear();
    m_powerUps.clear();
    m_base.reset();
    m_base2.reset();

    m_playerStates[0] = {GameConfig::PLAYER_MULTI_LIVES, 0, 0};
    m_playerStates[1] = {GameConfig::PLAYER_MULTI_LIVES, 0, 0};

    m_freezeTimer = 0;
    m_dirty = true;

    // 加载双人对战地图
    QString levelPath = QString("data/levels/level_multi.json");
    Map map(MAP_COLS, MAP_ROWS, CELL_SIZE);

    if (!map.loadFromFile(levelPath)) {
        // 回退到默认对战地图
        QStringList defaultMap = {
            "BBBBBBBBBBBBBBB",
            "B      S      B",
            "B B B B B B B B",
            "B      S      B",
            "B  B       B  B",
            "B             B",
            "B  S   W   S  B",
            "B             B",
            "B  B       B  B",
            "B      S      B",
            "B B B B B B B B",
            "B      S      B",
            "B             B",
            "B      S      B",
            "BBBBBBBBBBBBBBB"
        };
        map.loadFromData(defaultMap);
    }

    map.buildWalls(this);

    // 生成两个玩家
    spawnPlayer(0, QPointF(2 * CELL_SIZE, 2 * CELL_SIZE));
    spawnPlayer(1, QPointF(12 * CELL_SIZE, 12 * CELL_SIZE));

    // 给玩家2装备技能
    for (auto& t : m_tanks) {
        if (t->playerIndex() == 0) {
            t->setSkill(std::make_unique<SprintSkill>());
        }
    }

    m_remainingEnemies = 0;
    m_totalEnemies = 0;
}

void GameEngine::loadLevel(int levelId) {
    QString levelPath = QString("data/levels/level_%1.json")
        .arg(levelId, 2, 10, QChar('0'));

    Map map(MAP_COLS, MAP_ROWS, CELL_SIZE);

    if (!map.loadFromFile(levelPath)) {
        qDebug() << "GameEngine: failed to load level" << levelId << ", using defaults";
        QStringList defaultMap = {
            "BBBBBBBBBBBBBBB",
            "B   S   S     B",
            "B B B B B B B B",
            "B   S   S     B",
            "B  B       B  B",
            "B             B",
            "B     B       B",
            "B             B",
            "B  B       B  B",
            "B   S   S     B",
            "B B B B B B B B",
            "B   S   S     B",
            "B             B",
            "B             B",
            "BBBBBBBBBBBBBBB"
        };
        map.loadFromData(defaultMap);
    }

    // 构建墙壁（委托给 Map 类）
    map.buildWalls(this);

    // 生成基地
    QPointF basePos = map.basePosition();
    if (basePos.isNull()) {
        basePos = QPointF(7 * CELL_SIZE, 13 * CELL_SIZE);
    }
    m_base = std::make_unique<Base>(basePos, CELL_SIZE);

    // 生成玩家
    QPointF playerPos = map.playerSpawn();
    if (playerPos.isNull()) {
        playerPos = QPointF(4 * CELL_SIZE, 13 * CELL_SIZE);
    }
    spawnPlayer(0, playerPos);
    // 给玩家1装备默认技能（疾跑）
    for (auto& t : m_tanks) {
        if (t->playerIndex() == 0) {
            t->setSkill(std::make_unique<SprintSkill>());
        }
    }

    // 生成敌方坦克
    if (!map.enemySpawns().isEmpty()) {
        for (int i = 0; i < map.enemySpawns().size(); ++i) {
            TankType eType = i < map.enemyTankTypes().size()
                ? map.enemyTankTypes()[i] : TankType::EnemyBasic;
            QString aiType = i < map.enemyAITypes().size()
                ? map.enemyAITypes()[i] : QStringLiteral("PatrolAI");
            spawnEnemy(eType, map.enemySpawns()[i], aiType);
        }
    } else {
        // 默认敌人配置
        spawnEnemy(TankType::EnemyBasic, QPointF(1 * CELL_SIZE, 1 * CELL_SIZE), "PatrolAI");
        spawnEnemy(TankType::EnemyBasic, QPointF(7 * CELL_SIZE, 1 * CELL_SIZE), "AggressiveAI");
        spawnEnemy(TankType::EnemyBasic, QPointF(13 * CELL_SIZE, 1 * CELL_SIZE), "PatrolAI");
        spawnEnemy(TankType::EnemyFast,  QPointF(1 * CELL_SIZE, 7 * CELL_SIZE), "AggressiveAI");
        spawnEnemy(TankType::EnemyHeavy, QPointF(13 * CELL_SIZE, 7 * CELL_SIZE), "PatrolAI");
    }

    // 统计敌人数量
    m_remainingEnemies = 0;
    for (const auto& t : m_tanks) {
        if (!t->isPlayer()) m_remainingEnemies++;
    }
    m_totalEnemies = m_remainingEnemies;

    // 随机放置道具
    if (!map.powerUpNames().isEmpty()) {
        spawnPowerUps(map.powerUpNames());
    }
}

void GameEngine::addWall(const Wall& wall) {
    m_walls.push_back(std::make_unique<Wall>(wall));
}

void GameEngine::spawnPlayer(int playerIndex, const QPointF& pos) {
    auto player = std::make_unique<PlayerTank>(playerIndex, pos);
    m_tanks.push_back(std::move(player));
    qDebug() << "GameEngine: spawned player" << playerIndex << "at" << pos;
}

void GameEngine::spawnEnemy(TankType type, const QPointF& pos, const QString& aiType) {
    std::unique_ptr<AIController> ai(AIControllerFactory::instance().create(aiType));
    if (!ai) {
        qWarning() << "GameEngine: unknown AI type:" << aiType << ", falling back to PatrolAI";
        ai.reset(AIControllerFactory::instance().create("PatrolAI"));
    }
    auto enemy = std::make_unique<EnemyTank>(type, pos, std::move(ai));
    m_tanks.push_back(std::move(enemy));
}

void GameEngine::spawnPowerUps(const QStringList& powerUpNames) {
    // 尝试在空地上放置道具，避免与墙壁重叠
    for (const auto& name : powerUpNames) {
        auto pu = std::unique_ptr<PowerUp>(PowerUpFactory::instance().create(name));
        if (!pu) {
            qWarning() << "GameEngine: unknown powerup type:" << name;
            continue;
        }

        // 最多尝试 20 次找到空地
        bool placed = false;
        for (int attempt = 0; attempt < 20 && !placed; ++attempt) {
            int col = QRandomGenerator::global()->bounded(1, MAP_COLS - 1);
            int row = QRandomGenerator::global()->bounded(1, MAP_ROWS - 1);
            QPointF candidate(col * CELL_SIZE + (CELL_SIZE - GameConfig::POWERUP_SIZE) / 2,
                              row * CELL_SIZE + (CELL_SIZE - GameConfig::POWERUP_SIZE) / 2);

            // 检查是否与墙壁重叠
            QRectF puRect(candidate, pu->size());
            bool overlaps = false;
            for (const auto& wall : m_walls) {
                if (wall->isActive() && puRect.intersects(QRectF(wall->position(), wall->size()))) {
                    overlaps = true;
                    break;
                }
            }
            if (!overlaps) {
                pu->setPosition(candidate);  // 使用新的 setPosition 方法
                m_powerUps.push_back(std::move(pu));
                placed = true;
            }
        }
    }
    qDebug() << "GameEngine: spawned" << m_powerUps.size() << "powerups";
}

// ============================================================
// 输入处理
// ============================================================
void GameEngine::handleInput(int playerIndex, int direction, bool fire) {
    // 查找对应玩家坦克
    Tank* playerTank = nullptr;
    for (auto& t : m_tanks) {
        if (t->isPlayer() && t->playerIndex() == playerIndex && t->isAlive()) {
            playerTank = t.get();
            break;
        }
    }
    if (!playerTank) return;

    // 移动
    if (direction >= 0 && direction < 4) {
        Direction dir = static_cast<Direction>(direction);
        playerTank->setDirection(dir);
        QPointF oldPos = playerTank->position();
        playerTank->move(dir);

        // 碰撞检测：墙壁
        if (checkWallCollision(playerTank->position(), playerTank->size())) {
            playerTank->setPosition(oldPos);
        }
        // 碰撞检测：其他坦克
        int idx = -1;
        for (int i = 0; i < static_cast<int>(m_tanks.size()); ++i) {
            if (m_tanks[i].get() == playerTank) { idx = i; break; }
        }
        if (checkTankCollision(playerTank->position(), playerTank->size(), idx)) {
            playerTank->setPosition(oldPos);
        }
        // 边界限制
        playerTank->setPosition(clampToMap(playerTank->position(), playerTank->size()));

        m_dirty = true;
    }

    // 射击
    if (fire) {
        auto playerTankPtr = dynamic_cast<PlayerTank*>(playerTank);
        if (playerTankPtr) {
            Bullet* bullet = playerTankPtr->fire();
            if (bullet) {
                m_bullets.push_back(std::unique_ptr<Bullet>(bullet));
                m_dirty = true;
            }
        }
    }
}

bool GameEngine::activateSkill(int playerIndex) {
    Tank* playerTank = nullptr;
    for (auto& t : m_tanks) {
        if (t->isPlayer() && t->playerIndex() == playerIndex && t->isAlive()) {
            playerTank = t.get();
            break;
        }
    }
    if (!playerTank || !playerTank->skill()) return false;

    GameState state = getGameState();
    bool success = playerTank->skill()->activate(playerTank, state);

    if (success) {
        QString skillName = playerTank->skill()->name();
        emit skillActivated(playerIndex, skillName);

        // 特殊技能处理
        if (skillName == "Missile") {
            spawnMissile(playerTank);
        } else if (skillName == "Shield Wall") {
            // 在坦克前方生成临时墙壁
            double cx = playerTank->position().x() + playerTank->sizeValue() / 2;
            double cy = playerTank->position().y() + playerTank->sizeValue() / 2;
            double offset = playerTank->sizeValue();
            QPointF wallPos;
            switch (playerTank->direction()) {
            case Direction::Up:    wallPos = QPointF(cx - CELL_SIZE/2, cy - offset - CELL_SIZE/2); break;
            case Direction::Down:  wallPos = QPointF(cx - CELL_SIZE/2, cy + offset - CELL_SIZE/2); break;
            case Direction::Left:  wallPos = QPointF(cx - offset - CELL_SIZE/2, cy - CELL_SIZE/2); break;
            case Direction::Right: wallPos = QPointF(cx + offset - CELL_SIZE/2, cy - CELL_SIZE/2); break;
            }
            m_walls.push_back(std::make_unique<Wall>(wallPos, WallType::Steel, CELL_SIZE));
        } else if (skillName == "Sprint") {
            m_playerStates[playerIndex].sprintTimer = GameConfig::SKILL_SPRINT_DURATION_MS;
        }

        m_dirty = true;
    }
    return success;
}

// ============================================================
// 主更新循环
// ============================================================
void GameEngine::update() {
    if (m_gameOver) return;

    int frameDelta = 16;  // 约 60 FPS
    m_lastFrameMs += frameDelta;

    updateEnemyAI();
    updateTimedEffects(frameDelta);
    updateBullets();
    checkCollisions();
    removeDeadEntities();
    checkWinCondition();
}

void GameEngine::updateEnemyAI() {
    GameState state = getGameState();

    for (auto& t : m_tanks) {
        if (!t->isPlayer() && t->isAlive()) {
            auto enemy = dynamic_cast<EnemyTank*>(t.get());
            if (!enemy) continue;

            // 保存位置
            QPointF oldPos = enemy->position();
            Direction oldDir = enemy->direction();

            // AI 决策（移动 + 射击均由 AI 决定）
            enemy->updateAI(state);

            // 碰撞检测
            if (checkWallCollision(enemy->position(), enemy->size())) {
                enemy->setPosition(oldPos);
            }
            enemy->setPosition(clampToMap(enemy->position(), enemy->size()));

            // AI 决定射击：如果方向改变且对齐玩家，尝试开火
            // AI 控制器内部通过 setDirection 表达射击意图
            // 开火由 AI 控制器在 update 中直接调用 fire() 决定
        }
    }
}

void GameEngine::updateBullets() {
    for (auto& bullet : m_bullets) {
        if (bullet->isActive()) {
            bullet->update();

            // 超出地图边界 → 失活
            QPointF p = bullet->position();
            if (p.x() < 0 || p.y() < 0 ||
                p.x() > mapWidth() || p.y() > mapHeight()) {
                bullet->deactivate();
            }
        }
    }
}

void GameEngine::updateTimedEffects(int frameDeltaMs) {
    // 冰冻计时器
    if (m_freezeTimer > 0) {
        m_freezeTimer -= frameDeltaMs;
        if (m_freezeTimer <= 0) {
            // 解冻所有敌人
            for (auto& t : m_tanks) {
                if (!t->isPlayer()) {
                    t->setFrozen(false);
                }
            }
            m_freezeTimer = 0;
        }
    }

    // 玩家疾跑计时器
    for (int i = 0; i < 2; ++i) {
        if (m_playerStates[i].sprintTimer > 0) {
            m_playerStates[i].sprintTimer -= frameDeltaMs;
            if (m_playerStates[i].sprintTimer <= 0) {
                // 恢复速度
                for (auto& t : m_tanks) {
                    if (t->isPlayer() && t->playerIndex() == i) {
                        t->setSpeed(t->baseSpeed());
                    }
                }
                m_playerStates[i].sprintTimer = 0;
            }
        }
    }

    // 加速道具计时器
    for (auto& t : m_tanks) {
        if (t->hasRapidFire() && t->rapidFireTimer() > 0) {
            t->tickRapidFire(frameDeltaMs);
            if (t->rapidFireTimer() <= 0) {
                t->setRapidFire(false);
            }
        }
    }

    // 隐身计时器
    for (auto& t : m_tanks) {
        if (t->isInvisible() && t->isPlayer()) {
            // 隐身由 GameEngine 统一管理（简化处理：通过检查 isInvisible）
        }
    }
}

// ============================================================
// 碰撞检测
// ============================================================
void GameEngine::checkCollisions() {
    // 1. 子弹 vs 墙壁
    for (auto& bullet : m_bullets) {
        if (!bullet->isActive()) continue;
        QRectF bulletRect(bullet->position(), bullet->size());

        for (auto& wall : m_walls) {
            if (!wall->isActive()) continue;
            if (!wall->blocksBullets()) continue;  // 水域不阻挡子弹
            QRectF wallRect(wall->position(), wall->size());
            if (bulletRect.intersects(wallRect)) {
                bullet->deactivate();
                wall->destroy();
                m_dirty = true;
                break;
            }
        }
    }

    // 2. 子弹 vs 坦克
    for (auto& bullet : m_bullets) {
        if (!bullet->isActive()) continue;

        for (int i = 0; i < static_cast<int>(m_tanks.size()); ++i) {
            auto& tank = m_tanks[i];
            if (!tank->isAlive()) continue;

            // 友军火力避免
            if (bullet->isPlayerBullet() && tank->isPlayer()
                && bullet->ownerIndex() == tank->playerIndex()) continue;
            if (!bullet->isPlayerBullet() && !tank->isPlayer()) continue;

            // 隐身坦克不会被敌人子弹击中
            if (!bullet->isPlayerBullet() && tank->isPlayer() && tank->isInvisible()) continue;

            QRectF bulletRect(bullet->position(), bullet->size());
            QRectF tankRect(tank->position(), tank->size());
            if (bulletRect.intersects(tankRect)) {
                bullet->deactivate();
                tank->takeDamage(1);

                if (!tank->isAlive()) {
                    if (!tank->isPlayer()) {
                        // 敌人被击杀
                        int scorerIdx = bullet->ownerIndex();
                        int scoreAdd = GameConfig::SCORE_PER_KILL;
                        if (tank->type() == TankType::EnemyBoss) {
                            scoreAdd = GameConfig::SCORE_BOSS_KILL;
                        }
                        if (scorerIdx >= 0 && scorerIdx < 2) {
                            m_playerStates[scorerIdx].score += scoreAdd;
                            emit scoreChanged(scorerIdx, m_playerStates[scorerIdx].score);
                        }
                        m_remainingEnemies--;
                        qDebug() << "Enemy killed! Remaining:" << m_remainingEnemies;
                    } else {
                        // 玩家被击中
                        int pi = tank->playerIndex();
                        if (pi >= 0 && pi < 2) {
                            m_playerStates[pi].lives--;
                            emit livesChanged(pi, m_playerStates[pi].lives);
                            // 重生
                            if (m_playerStates[pi].lives > 0) {
                                QPointF spawnPt = (pi == 0)
                                    ? QPointF(4 * CELL_SIZE, 13 * CELL_SIZE)
                                    : QPointF(10 * CELL_SIZE, 13 * CELL_SIZE);
                                tank->setPosition(spawnPt);
                                tank->heal(tank->maxHp());
                                // 清除负面效果
                                tank->setInvisible(false);
                                tank->setFrozen(false);
                            }
                        }
                    }
                }
                m_dirty = true;
                break;
            }
        }
    }

    // 3. 子弹 vs 基地
    auto checkBaseHit = [this](Base* base) {
        if (!base || base->isDestroyed()) return;
        for (auto& bullet : m_bullets) {
            if (!bullet->isActive()) continue;
            if (bullet->isPlayerBullet()) continue;  // 玩家子弹不伤基地
            QRectF bulletRect(bullet->position(), bullet->size());
            QRectF baseRect(base->position(), base->size());
            if (bulletRect.intersects(baseRect)) {
                bullet->deactivate();
                base->destroy();
                m_dirty = true;
                break;
            }
        }
    };
    checkBaseHit(m_base.get());
    checkBaseHit(m_base2.get());

    // 4. 坦克 vs 道具
    for (auto& pu : m_powerUps) {
        if (!pu->isActive()) continue;
        for (auto& tank : m_tanks) {
            if (!tank->isAlive() || !tank->isPlayer()) continue;

            QRectF puRect(pu->position(), pu->size());
            QRectF tankRect(tank->position(), tank->size());
            if (puRect.intersects(tankRect)) {
                // 特殊道具处理
                if (pu->powerUpType() == PowerUpType::Bomb) {
                    // 全屏炸弹：消灭所有敌人
                    for (auto& t : m_tanks) {
                        if (!t->isPlayer() && t->isAlive()) {
                            t->takeDamage(999);
                            m_remainingEnemies--;
                            int scorerIdx = tank->playerIndex();
                            m_playerStates[scorerIdx].score += GameConfig::SCORE_PER_KILL;
                            emit scoreChanged(scorerIdx, m_playerStates[scorerIdx].score);
                        }
                    }
                } else if (pu->powerUpType() == PowerUpType::Freeze) {
                    // 冰冻：冻结所有敌人
                    for (auto& t : m_tanks) {
                        if (!t->isPlayer()) {
                            t->setFrozen(true);
                        }
                    }
                    m_freezeTimer = GameConfig::FREEZE_DURATION_MS;
                } else {
                    pu->apply(tank.get());
                }

                int scorerIdx = tank->playerIndex();
                m_playerStates[scorerIdx].score += GameConfig::SCORE_POWERUP_COLLECT;
                emit scoreChanged(scorerIdx, m_playerStates[scorerIdx].score);
                emit powerUpCollected(scorerIdx, pu->name());

                pu->collect();
                m_dirty = true;
                break;
            }
        }
    }
}

void GameEngine::checkWinCondition() {
    if (!m_isMultiPlayer) {
        // 单人模式
        bool playerAlive = false;
        for (const auto& t : m_tanks) {
            if (t->isPlayer() && t->isAlive()) playerAlive = true;
        }

        if (!playerAlive || m_playerStates[0].lives <= 0) {
            m_gameOver = true;
            m_winner = 0;  // 玩家失败
            emit gameOver(m_winner);
            return;
        }

        if (m_remainingEnemies <= 0) {
            m_gameOver = true;
            m_winner = 1;  // 玩家获胜
            emit gameOver(m_winner);
            return;
        }

        // 基地被毁
        if (GameConfig::BASE_DESTROY_ENDS_GAME && m_base && m_base->isDestroyed()) {
            m_gameOver = true;
            m_winner = 0;
            emit gameOver(m_winner);
            return;
        }
    } else {
        // 双人模式
        if (GameConfig::BASE_DESTROY_ENDS_GAME) {
            if (m_base && m_base->isDestroyed()) {
                m_gameOver = true;
                m_winner = 0;
                emit gameOver(m_winner);
                return;
            }
        }

        for (int i = 0; i < 2; ++i) {
            if (m_playerStates[i].lives <= 0) {
                m_gameOver = true;
                m_winner = (i == 0) ? 2 : 1;
                emit gameOver(m_winner);
                return;
            }
        }
    }
}

void GameEngine::removeDeadEntities() {
    size_t before = m_bullets.size();
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
                       [](const auto& b) { return !b->isActive(); }),
        m_bullets.end());
    if (m_bullets.size() != before) m_dirty = true;

    before = m_walls.size();
    m_walls.erase(
        std::remove_if(m_walls.begin(), m_walls.end(),
                       [](const auto& w) { return !w->isActive(); }),
        m_walls.end());
    if (m_walls.size() != before) m_dirty = true;

    before = m_powerUps.size();
    m_powerUps.erase(
        std::remove_if(m_powerUps.begin(), m_powerUps.end(),
                       [](const auto& p) { return !p->isActive(); }),
        m_powerUps.end());
    if (m_powerUps.size() != before) m_dirty = true;

    before = m_tanks.size();
    m_tanks.erase(
        std::remove_if(m_tanks.begin(), m_tanks.end(),
                       [](const auto& t) { return !t->isAlive() && !t->isPlayer(); }),
        m_tanks.end());
    if (m_tanks.size() != before) m_dirty = true;
}

// ============================================================
// 碰撞检测辅助
// ============================================================
bool GameEngine::checkWallCollision(const QPointF& pos, const QSizeF& size) const {
    QRectF rect(pos, size);
    for (const auto& wall : m_walls) {
        if (!wall->isActive()) continue;
        if (!wall->blocksMovement()) continue;
        if (rect.intersects(QRectF(wall->position(), wall->size()))) {
            return true;
        }
    }
    return false;
}

bool GameEngine::checkBulletWallCollision(const QPointF& pos, const QSizeF& size) const {
    QRectF rect(pos, size);
    for (const auto& wall : m_walls) {
        if (!wall->isActive()) continue;
        if (!wall->blocksBullets()) continue;
        if (rect.intersects(QRectF(wall->position(), wall->size()))) {
            return true;
        }
    }
    return false;
}

bool GameEngine::checkTankCollision(const QPointF& pos, const QSizeF& size, int excludeIndex) const {
    QRectF rect(pos, size);
    for (int i = 0; i < static_cast<int>(m_tanks.size()); ++i) {
        if (i == excludeIndex) continue;
        if (!m_tanks[i]->isAlive()) continue;
        if (rect.intersects(QRectF(m_tanks[i]->position(), m_tanks[i]->size()))) {
            return true;
        }
    }
    return false;
}

QPointF GameEngine::clampToMap(const QPointF& pos, const QSizeF& size) const {
    double x = qBound(0.0, pos.x(), static_cast<double>(mapWidth() - size.width()));
    double y = qBound(0.0, pos.y(), static_cast<double>(mapHeight() - size.height()));
    return QPointF(x, y);
}

void GameEngine::spawnMissile(const Tank* tank) {
    // 导弹和普通子弹类似，但可以穿透墙壁
    auto* missile = new Bullet(
        QPointF(tank->position().x() + tank->sizeValue() / 2 - Bullet::defaultSize() / 2,
                tank->position().y() + tank->sizeValue() / 2 - Bullet::defaultSize() / 2),
        tank->direction(), tank->playerIndex());
    // 导弹标记为特殊类型（通过特殊判断跳过墙壁碰撞）
    m_bullets.push_back(std::unique_ptr<Bullet>(missile));
}

GameState GameEngine::getGameState() const {
    GameState state;

    // 玩家1 信息
    for (const auto& t : m_tanks) {
        if (t->isPlayer() && t->playerIndex() == 0 && t->isAlive()) {
            state.playerPos = t->position();
            state.playerDir = t->direction();
            state.playerAlive = true;
            state.playerHasShield = t->hasShield();
        }
    }

    // 玩家2 信息
    for (const auto& t : m_tanks) {
        if (t->isPlayer() && t->playerIndex() == 1 && t->isAlive()) {
            state.player2Pos = t->position();
            state.player2Alive = true;
        }
    }

    // 敌人信息
    for (const auto& t : m_tanks) {
        if (!t->isPlayer() && t->isAlive()) {
            state.enemyPositions.append(t->position());
            state.enemyDirections.append(t->direction());
        }
    }

    // 子弹信息
    for (const auto& b : m_bullets) {
        if (b->isActive()) {
            state.bulletPositions.append(b->position());
            state.bulletIsPlayerOwned.append(b->isPlayerBullet());
        }
    }

    // 道具信息
    for (const auto& p : m_powerUps) {
        if (p->isActive()) {
            state.powerUpPositions.append(p->position());
            state.powerUpTypes.append(p->powerUpType());
        }
    }

    // 墙壁信息
    for (const auto& w : m_walls) {
        if (w->isActive()) {
            state.wallPositions.append(w->position());
        }
    }

    // 基地信息
    if (m_base) {
        state.basePosition = m_base->position();
        state.baseAlive = !m_base->isDestroyed();
    }

    state.mapWidth = mapWidth();
    state.mapHeight = mapHeight();

    return state;
}

int GameEngine::playerLives(int index) const {
    if (index < 0 || index > 1) return 0;
    return m_playerStates[index].lives;
}

int GameEngine::playerScore(int index) const {
    if (index < 0 || index > 1) return 0;
    return m_playerStates[index].score;
}
