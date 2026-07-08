/**
 * @file highscore_manager.cpp
 * @brief 高分榜管理器实现 —— SQLite CRUD
 */

#include "highscore_manager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QUuid>

HighScoreManager::HighScoreManager(const QString& dbPath)
    : m_dbPath(dbPath)
{
    m_connectionName = QUuid::createUuid().toString();
}

HighScoreManager::~HighScoreManager() {
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::database(m_connectionName).close();
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool HighScoreManager::initialize() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        qWarning() << "HighScoreManager: cannot open database:" << db.lastError().text();
        return false;
    }
    QSqlQuery query(db);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS highscores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            player TEXT NOT NULL,
            score INTEGER NOT NULL,
            mode TEXT NOT NULL,
            date TEXT NOT NULL
        )
    )");
    qDebug() << "HighScoreManager: initialized";
    return true;
}

bool HighScoreManager::addScore(const QString& player, int score, const QString& mode) {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return false;
    QSqlQuery query(db);
    query.prepare("INSERT INTO highscores (player, score, mode, date) VALUES (?, ?, ?, ?)");
    query.addBindValue(player);
    query.addBindValue(score);
    query.addBindValue(mode);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    if (!query.exec()) {
        qWarning() << "HighScoreManager: insert failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<HighScoreEntry> HighScoreManager::getTopScores(int limit, const QString& mode) const {
    QVector<HighScoreEntry> results;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isOpen()) return results;
    QSqlQuery query(db);
    QString sql = "SELECT id, player, score, mode, date FROM highscores";
    if (!mode.isEmpty()) sql += " WHERE mode = ?";
    sql += " ORDER BY score DESC LIMIT ?";
    query.prepare(sql);
    if (!mode.isEmpty()) query.addBindValue(mode);
    query.addBindValue(limit);
    if (!query.exec()) return results;
    while (query.next()) {
        HighScoreEntry e;
        e.id = query.value(0).toInt();
        e.player = query.value(1).toString();
        e.score = query.value(2).toInt();
        e.mode = query.value(3).toString();
        e.date = query.value(4).toString();
        results.append(e);
    }
    return results;
}

bool HighScoreManager::isNewHighScore(int score, const QString& mode) const {
    auto scores = getTopScores(1, mode);
    if (scores.isEmpty()) return true;
    return score > scores.first().score;
}
