#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QSet>
#include <QElapsedTimer>
#include "constants.h"
#include "mapdata.h"
#include "enemytank.h"

class Tank;
class PlayerTank;
class EnemyTank;
class Bullet;
class Wall;
class GameManager;

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GameScene(GameManager* manager, QObject* parent = nullptr);
    ~GameScene();

    // 游戏控制
    void startSinglePlayer(int level = 0);
    void startPvP();
    void pause();
    void resume();
    void stop();
    bool isRunning() const { return m_running; }

    // 输入处理
    void keyPressed(int key);
    void keyReleased(int key);

    // 获取游戏信息
    PlayerTank* player1() const { return m_player1; }
    PlayerTank* player2() const { return m_player2; }

signals:
    void gameOver(bool won);
    void scoreChanged(int p1, int p2);

private slots:
    void gameLoop();

private:
    // 地图
    void loadMap(const int map[MAP_ROWS][MAP_COLS]);
    void clearMap();

    // 生成
    void spawnPlayer1();
    void spawnPlayer2();
    void spawnEnemies(int count, EnemyTank::Difficulty diff);

    // 碰撞检测
    void checkTankWallCollision(Tank* tank);
    void checkBulletCollisions();
    void checkTankTankCollisions();
    Wall* wallAt(int row, int col) const;
    bool isBlocked(qreal x, qreal y) const;
    QList<QPoint> getOverlappingCells(const QRectF& rect) const;

    // 更新
    void updateEnemyAI();
    void removeDeadObjects();

    // 地图数据
    Wall* m_wallGrid[MAP_ROWS][MAP_COLS];

    // 游戏对象
    PlayerTank* m_player1;
    PlayerTank* m_player2;
    QList<EnemyTank*> m_enemies;
    QList<Bullet*> m_bullets;

    // 游戏状态
    GameManager* m_manager;
    QTimer* m_timer;
    QSet<int> m_keysPressed;
    bool m_running;
    GameMode m_mode;
};

#endif // GAMESCENE_H
