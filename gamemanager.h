#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "constants.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QObject* parent = nullptr);

    // 游戏状态
    enum State { WAITING, PLAYING, PAUSED, WON, LOST };
    State state() const { return m_state; }
    void setState(State s);

    // 单人模式
    int currentLevel() const { return m_currentLevel; }
    int totalLevels() const { return m_totalLevels; }
    int enemiesRemaining() const { return m_enemiesRemaining; }
    void setEnemiesRemaining(int n) { m_enemiesRemaining = n; }
    void enemyKilled();

    void startSinglePlayer();
    void beginLevel();   // 仅切换到PLAYING状态，不重置关卡
    void nextLevel();
    bool hasNextLevel() const;

    // 双人模式
    int player1Score() const { return m_p1Score; }
    int player2Score() const { return m_p2Score; }
    void player1Scored() { m_p1Score++; }
    void player2Scored() { m_p2Score++; }
    void startPvP();
    bool isPvPOver() const;

    // 通用
    void reset();
    void pause();
    void resume();

signals:
    void stateChanged(GameManager::State newState);
    void levelChanged(int level);
    void scoreChanged(int p1Score, int p2Score);

private:
    State m_state;
    GameMode m_mode;
    int m_currentLevel;
    int m_totalLevels;
    int m_enemiesRemaining;
    int m_p1Score;
    int m_p2Score;
};

#endif // GAMEMANAGER_H
