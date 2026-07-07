#include "gamemanager.h"
#include "mapdata.h"

GameManager::GameManager(QObject* parent)
    : QObject(parent)
    , m_state(WAITING)
    , m_mode(MODE_SINGLE)
    , m_currentLevel(0)
    , m_totalLevels(999)  // 无限关卡（随机生成）
    , m_enemiesRemaining(0)
    , m_p1Score(0)
    , m_p2Score(0)
{
}

void GameManager::setState(State s)
{
    if (m_state != s) {
        m_state = s;
        emit stateChanged(s);
    }
}

void GameManager::startSinglePlayer()
{
    m_mode = MODE_SINGLE;
    m_currentLevel = 0;
    m_p1Score = 0;
    m_p2Score = 0;
    setState(PLAYING);
    emit levelChanged(m_currentLevel + 1);
}

void GameManager::beginLevel()
{
    setState(PLAYING);
    emit levelChanged(m_currentLevel + 1);
}

void GameManager::enemyKilled()
{
    m_enemiesRemaining--;
    if (m_enemiesRemaining <= 0) {
        m_p1Score += 100; // 过关奖励分
        emit scoreChanged(m_p1Score, m_p2Score);
        // 不在这里推进关卡 — 由 GameScene 的 gameLoop() 处理
    }
}

void GameManager::nextLevel()
{
    m_currentLevel++;
    emit levelChanged(m_currentLevel + 1);
}

bool GameManager::hasNextLevel() const
{
    return m_currentLevel + 1 < m_totalLevels;
}

void GameManager::startPvP()
{
    m_mode = MODE_PVP;
    m_p1Score = 0;
    m_p2Score = 0;
    m_currentLevel = 0;
    setState(PLAYING);
}

bool GameManager::isPvPOver() const
{
    return m_p1Score >= WIN_SCORE_PVP || m_p2Score >= WIN_SCORE_PVP;
}

void GameManager::reset()
{
    m_state = WAITING;
    m_currentLevel = 0;
    m_p1Score = 0;
    m_p2Score = 0;
    m_enemiesRemaining = 0;
}

void GameManager::pause()
{
    if (m_state == PLAYING)
        setState(PAUSED);
}

void GameManager::resume()
{
    if (m_state == PAUSED)
        setState(PLAYING);
}
