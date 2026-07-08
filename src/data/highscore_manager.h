/**
 * @file highscore_manager.h
 * @brief 高分榜管理器 —— SQLite 存储，支持单人/双人模式分别排行
 */

#pragma once

#include <QString>
#include <QVector>
#include <QSqlDatabase>

struct HighScoreEntry {
    int id = 0;
    QString player;
    int score = 0;
    QString mode;  // "single" / "multi"
    QString date;
};

class HighScoreManager {
public:
    explicit HighScoreManager(const QString& dbPath = "data/highscores.db");
    ~HighScoreManager();

    bool initialize();
    bool addScore(const QString& player, int score, const QString& mode);
    QVector<HighScoreEntry> getTopScores(int limit = 10, const QString& mode = "") const;
    bool isNewHighScore(int score, const QString& mode = "") const;

private:
    QString m_dbPath;
    QString m_connectionName;
};
