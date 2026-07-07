#include "gamescene.h"
#include "tank.h"
#include "playertank.h"
#include "enemytank.h"
#include "bullet.h"
#include "wall.h"
#include "gamemanager.h"
#include <QtMath>
#include <algorithm>

GameScene::GameScene(GameManager* manager, QObject* parent)
    : QGraphicsScene(parent)
    , m_player1(nullptr)
    , m_player2(nullptr)
    , m_manager(manager)
    , m_running(false)
    , m_mode(MODE_SINGLE)
{
    // 初始化场景
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    setBackgroundBrush(GameColors::BACKGROUND);

    // 初始化墙网格
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            m_wallGrid[r][c] = nullptr;

    // 创建游戏循环定时器
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameScene::gameLoop);
}

GameScene::~GameScene()
{
    stop();
}

// ===== 游戏控制 =====

void GameScene::startSinglePlayer(int level)
{
    stop();
    m_mode = MODE_SINGLE;
    m_running = true;

    // 随机生成地图（墙壁密度随关卡递增）
    int map[MAP_ROWS][MAP_COLS];
    generateRandomMap(map, level);
    loadMap(map);

    // 生成玩家1
    spawnPlayer1();

    // 敌人数随关卡递增：4 + level，上限 16
    int enemyCount = 4 + level;
    if (enemyCount > 16) enemyCount = 16;

    // 难度递进
    EnemyTank::Difficulty diff;
    if (level == 0)
        diff = EnemyTank::EASY;
    else if (level <= 2)
        diff = EnemyTank::NORMAL;
    else
        diff = EnemyTank::HARD;

    spawnEnemies(enemyCount, diff);

    m_manager->setEnemiesRemaining(enemyCount);
    m_manager->beginLevel();

    m_timer->start(FRAME_INTERVAL);
}

void GameScene::startPvP()
{
    stop();
    m_mode = MODE_PVP;
    m_running = true;

    // 加载PVP地图
    loadMap(MAP_PVP);

    // 生成两个玩家
    spawnPlayer1();
    spawnPlayer2();

    m_manager->startPvP();
    m_timer->start(FRAME_INTERVAL);
}

void GameScene::pause()
{
    if (m_running) {
        m_timer->stop();
        m_manager->pause();
    }
}

void GameScene::resume()
{
    if (!m_running && m_manager->state() == GameManager::PAUSED) {
        m_running = true;
        m_manager->resume();
        m_timer->start(FRAME_INTERVAL);
    }
}

void GameScene::stop()
{
    m_timer->stop();
    m_running = false;
    clearMap();

    // 清理玩家坦克（Qt 会自动删除其子子弹）
    if (m_player1) {
        removeItem(m_player1);
        delete m_player1;
        m_player1 = nullptr;
    }
    if (m_player2) {
        removeItem(m_player2);
        delete m_player2;
        m_player2 = nullptr;
    }

    // 清理敌人坦克（Qt 会自动删除其子子弹）
    for (auto* enemy : m_enemies) {
        removeItem(enemy);
        delete enemy;
    }
    m_enemies.clear();

    // 清理残留的孤儿子弹（没有父坦克的）
    QList<QGraphicsItem*> remaining = items();
    for (auto* item : remaining) {
        if (auto* bullet = dynamic_cast<Bullet*>(item)) {
            removeItem(bullet);
            delete bullet;
        }
    }

    m_bullets.clear();
    m_keysPressed.clear();
}

// ===== 输入处理 =====

void GameScene::keyPressed(int key)
{
    m_keysPressed.insert(key);

    // 暂停键
    if (key == Qt::Key_P || key == Qt::Key_Escape) {
        if (m_running)
            pause();
        else
            resume();
    }
}

void GameScene::keyReleased(int key)
{
    m_keysPressed.remove(key);
}

// ===== 游戏主循环 =====

void GameScene::gameLoop()
{
    if (!m_running) return;
    if (m_manager->state() != GameManager::PLAYING) return;

    // 1. 处理玩家输入
    if (m_player1) {
        m_player1->processInput(m_keysPressed);
        checkTankWallCollision(m_player1);
    }
    if (m_player2) {
        m_player2->processInput(m_keysPressed);
        checkTankWallCollision(m_player2);
    }

    // 2. 更新敌人AI
    updateEnemyAI();

    // 3. 收集所有子弹
    m_bullets.clear();
    for (auto* item : items()) {
        if (auto* bullet = dynamic_cast<Bullet*>(item)) {
            if (bullet->isActive())
                m_bullets.append(bullet);
        }
    }

    // 4. 移动子弹并检测碰撞
    for (auto* bullet : m_bullets) {
        bullet->move();
    }
    checkBulletCollisions();

    // 5. 坦克间碰撞
    checkTankTankCollisions();

    // 6. 清理死亡对象
    removeDeadObjects();

    // 7. 检查胜负
    if (m_mode == MODE_SINGLE) {
        // 检查玩家是否死亡
        if (m_player1 && !m_player1->isAlive()) {
            m_running = false;
            m_timer->stop();
            m_manager->setState(GameManager::LOST);
            emit gameOver(false);
            return;
        }

        // 所有敌人被消灭 → 过关
        if (m_enemies.isEmpty() && m_manager->enemiesRemaining() <= 0) {
            if (m_manager->hasNextLevel()) {
                int nextLevel = m_manager->currentLevel() + 1;
                m_manager->nextLevel();
                startSinglePlayer(nextLevel);
            } else {
                m_running = false;
                m_timer->stop();
                m_manager->setState(GameManager::WON);
                emit gameOver(true);
            }
        }
    } else if (m_mode == MODE_PVP) {
        // PvP: 检查是否有人达到目标分数
        if (m_manager->isPvPOver()) {
            m_running = false;
            m_timer->stop();
            bool p1Wins = m_manager->player1Score() >= WIN_SCORE_PVP;
            m_manager->setState(p1Wins ? GameManager::WON : GameManager::LOST);
            emit gameOver(p1Wins);
        }

        // 如果有玩家死亡，重生
        if (m_player1 && !m_player1->isAlive()) {
            m_manager->player2Scored();
            emit scoreChanged(m_manager->player1Score(), m_manager->player2Score());
            if (!m_manager->isPvPOver()) {
                m_player1->respawn(SCENE_WIDTH / 4, SCENE_HEIGHT - TILE_SIZE);
            }
        }
        if (m_player2 && !m_player2->isAlive()) {
            m_manager->player1Scored();
            emit scoreChanged(m_manager->player1Score(), m_manager->player2Score());
            if (!m_manager->isPvPOver()) {
                m_player2->respawn(SCENE_WIDTH * 3 / 4, SCENE_HEIGHT - TILE_SIZE);
            }
        }
    }
}

// ===== 地图 =====

void GameScene::loadMap(const int map[MAP_ROWS][MAP_COLS])
{
    clearMap();

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int cellType = map[r][c];
            if (cellType == CELL_EMPTY) continue;

            if (cellType >= CELL_BRICK && cellType <= CELL_GRASS) {
                Wall* wall = new Wall(r, c, static_cast<MapCell>(cellType));
                addItem(wall);
                m_wallGrid[r][c] = wall;
            }
        }
    }
}

void GameScene::clearMap()
{
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (m_wallGrid[r][c]) {
                removeItem(m_wallGrid[r][c]);
                delete m_wallGrid[r][c];
                m_wallGrid[r][c] = nullptr;
            }
        }
    }

}

// ===== 生成 =====

void GameScene::spawnPlayer1()
{
    qreal x = TILE_SIZE * 4.5;   // 左下区域
    qreal y = SCENE_HEIGHT - TILE_SIZE * 1.5;
    m_player1 = new PlayerTank(x, y, 1);
    addItem(m_player1);
}

void GameScene::spawnPlayer2()
{
    qreal x = SCENE_WIDTH - TILE_SIZE * 4.5;  // 右下区域
    qreal y = SCENE_HEIGHT - TILE_SIZE * 1.5;
    m_player2 = new PlayerTank(x, y, 2);
    addItem(m_player2);
}

void GameScene::spawnEnemies(int count, EnemyTank::Difficulty diff)
{
    // 在顶部区域随机生成敌人
    int spawnRows[] = {0, 1, 2};
    int spawnCols[] = {0, 4, 8, 12, 16};

    for (int i = 0; i < count; i++) {
        int row = spawnRows[i % 3];
        int col = spawnCols[i % 5];
        qreal x = col * TILE_SIZE + TILE_SIZE / 2.0;
        qreal y = row * TILE_SIZE + TILE_SIZE / 2.0;

        auto* enemy = new EnemyTank(x, y, diff);
        addItem(enemy);
        m_enemies.append(enemy);
    }
}

// ===== 碰撞检测 =====

Wall* GameScene::wallAt(int row, int col) const
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
        return nullptr;
    return m_wallGrid[row][col];
}

bool GameScene::isBlocked(qreal x, qreal y) const
{
    int col = static_cast<int>(x / TILE_SIZE);
    int row = static_cast<int>(y / TILE_SIZE);

    Wall* wall = wallAt(row, col);
    if (wall && wall->blocksTank())
        return true;

    return false;
}

QList<QPoint> GameScene::getOverlappingCells(const QRectF& rect) const
{
    QList<QPoint> cells;

    int startCol = qMax(0, static_cast<int>(rect.left() / TILE_SIZE));
    int endCol = qMin(MAP_COLS - 1, static_cast<int>((rect.right() - 0.01) / TILE_SIZE));
    int startRow = qMax(0, static_cast<int>(rect.top() / TILE_SIZE));
    int endRow = qMin(MAP_ROWS - 1, static_cast<int>((rect.bottom() - 0.01) / TILE_SIZE));

    for (int r = startRow; r <= endRow; r++) {
        for (int c = startCol; c <= endCol; c++) {
            cells.append(QPoint(c, r));
        }
    }

    return cells;
}

void GameScene::checkTankWallCollision(Tank* tank)
{
    if (!tank || !tank->isAlive()) return;

    QRectF tankRect = tank->mapRectToScene(tank->boundingRect());
    QList<QPoint> cells = getOverlappingCells(tankRect);

    for (const QPoint& cell : cells) {
        Wall* wall = wallAt(cell.y(), cell.x());
        if (wall && wall->blocksTank()) {
            // 推回坦克：取消移动
            // 通过对比当前方向和墙的位置来推回
            QRectF wallRect(
                cell.x() * TILE_SIZE,
                cell.y() * TILE_SIZE,
                TILE_SIZE, TILE_SIZE
            );

            QRectF intersection = tankRect.intersected(wallRect);
            if (intersection.isEmpty()) continue;

            // 根据重叠情况推回
            // 如果坦克主要在上方 (向上移动撞到下面的墙)
            qreal overlapLeft = intersection.width();
            qreal overlapTop = intersection.height();

            // 找到最小推回距离的方向
            if (overlapLeft < overlapTop) {
                // 水平推回
                if (tank->x() < wallRect.center().x())
                    tank->setPos(tank->x() - overlapLeft, tank->y());
                else
                    tank->setPos(tank->x() + overlapLeft, tank->y());
            } else {
                // 垂直推回
                if (tank->y() < wallRect.center().y())
                    tank->setPos(tank->x(), tank->y() - overlapTop);
                else
                    tank->setPos(tank->x(), tank->y() + overlapTop);
            }

            // 更新碰撞矩形
            tankRect = tank->mapRectToScene(tank->boundingRect());
        }
    }

    // 确保坦克不超出边界
    qreal half = TANK_SIZE / 2.0;
    qreal newX = qBound(half, tank->x(), SCENE_WIDTH - half);
    qreal newY = qBound(half, tank->y(), SCENE_HEIGHT - half);
    tank->setPos(newX, newY);
}

void GameScene::checkBulletCollisions()
{
    for (auto* bullet : m_bullets) {
        if (!bullet->isActive()) continue;

        // 子弹与墙壁碰撞
        int col = static_cast<int>(bullet->x() / TILE_SIZE);
        int row = static_cast<int>(bullet->y() / TILE_SIZE);

        Wall* wall = wallAt(row, col);
        if (wall) {
            if (wall->isDestructible()) {
                // 砖墙：受到伤害（3次才摧毁）
                bool destroyed = wall->takeDamage();
                if (destroyed) {
                    m_wallGrid[row][col] = nullptr; // wall已被destroy()删除
                }
                bullet->bounce(row, col);  // 碰墙反射（光的反射定律）
                continue;
            } else if (wall->blocksBullet()) {
                // 钢铁墙：反射子弹
                bullet->bounce(row, col);
                continue;
            }
            // 水域和草丛不阻挡子弹
        }

        // 子弹与坦克碰撞
        QList<QGraphicsItem*> collidingItems = bullet->collidingItems();
        for (auto* item : collidingItems) {
            auto* tank = dynamic_cast<Tank*>(item);
            if (!tank || tank == bullet->owner()) continue;

            // 检查友军伤害
            bool friendlyFire = false;
            if (dynamic_cast<PlayerTank*>(bullet->owner()) && dynamic_cast<PlayerTank*>(tank))
                friendlyFire = true; // PVP中允许玩家互伤，这里不阻止
            if (dynamic_cast<EnemyTank*>(bullet->owner()) && dynamic_cast<EnemyTank*>(tank))
                friendlyFire = true;  // 敌人之间不会互相伤害

            // 单人模式下阻止友军伤害
            if (friendlyFire && m_mode == MODE_SINGLE)
                continue;

            // ★ 在 deactivate 前保存子弹主人指针
            Tank* bulletOwner = bullet->owner();

            tank->takeDamage();
            bullet->deactivate();

            // 敌人被击杀 → 立即清理
            if (auto* enemy = dynamic_cast<EnemyTank*>(tank)) {
                if (!enemy->isAlive()) {
                    m_enemies.removeOne(enemy);
                    removeItem(enemy);
                    delete enemy;
                    if (auto* p = dynamic_cast<PlayerTank*>(bulletOwner)) {
                        p->addScore();
                    }
                    m_manager->enemyKilled();
                }
            }

            break;  // 一颗子弹只命中一个目标
        }
    }
}

void GameScene::checkTankTankCollisions()
{
    // 收集所有存活的坦克
    QList<Tank*> allTanks;
    if (m_player1 && m_player1->isAlive()) allTanks.append(m_player1);
    if (m_player2 && m_player2->isAlive()) allTanks.append(m_player2);
    for (auto* enemy : m_enemies) {
        if (enemy->isAlive()) allTanks.append(enemy);
    }

    // 两两检测
    for (int i = 0; i < allTanks.size(); i++) {
        for (int j = i + 1; j < allTanks.size(); j++) {
            Tank* a = allTanks[i];
            Tank* b = allTanks[j];

            if (a->collidesWithItem(b)) {
                // 推开双方
                qreal dx = b->x() - a->x();
                qreal dy = b->y() - a->y();
                qreal dist = qSqrt(dx * dx + dy * dy);
                if (dist < 0.01) {
                    dx = 1; dy = 0;
                    dist = 1;
                }

                qreal overlap = TANK_SIZE - dist;
                qreal pushX = dx / dist * overlap / 2.0;
                qreal pushY = dy / dist * overlap / 2.0;

                a->setPos(a->x() - pushX, a->y() - pushY);
                b->setPos(b->x() + pushX, b->y() + pushY);

                // 边界钳制
                qreal half = TANK_SIZE / 2.0;
                a->setPos(qBound(half, a->x(), SCENE_WIDTH - half),
                           qBound(half, a->y(), SCENE_HEIGHT - half));
                b->setPos(qBound(half, b->x(), SCENE_WIDTH - half),
                           qBound(half, b->y(), SCENE_HEIGHT - half));
            }
        }
    }
}

// ===== 更新 =====

void GameScene::updateEnemyAI()
{
    QPointF playerPos = m_player1 ? m_player1->pos() : QPointF(SCENE_WIDTH/2, SCENE_HEIGHT);

    for (auto* enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        enemy->updateAI(playerPos);
        checkTankWallCollision(enemy);
    }
}

void GameScene::removeDeadObjects()
{
    // 移除死亡的敌人（安全网：正常流程中已在 checkBulletCollisions 中清理）
    for (int i = m_enemies.size() - 1; i >= 0; i--) {
        if (!m_enemies[i]->isAlive()) {
            removeItem(m_enemies[i]);
            delete m_enemies[i];
            m_enemies.removeAt(i);
        }
    }

    // 移除并删除无效子弹
    for (int i = m_bullets.size() - 1; i >= 0; i--) {
        if (!m_bullets[i]->isActive()) {
            delete m_bullets[i];
            m_bullets.removeAt(i);
        }
    }
}
