/**
 * @file map.cpp
 * @brief 地图加载实现
 */

#include "map.h"
#include "wall.h"
#include "game_engine.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Map::Map(int cols, int rows, int cellSize)
    : m_cols(cols)
    , m_rows(rows)
    , m_cellSize(cellSize)
{
}

bool Map::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: cannot open file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Map: invalid JSON";
        return false;
    }

    QJsonObject root = doc.object();

    // 关卡元数据
    m_levelId = root["levelId"].toInt(1);
    m_levelName = root["name"].toString(QString("Level %1").arg(m_levelId));
    m_levelDescription = root["description"].toString("");

    // 地图布局
    QJsonArray mapArr = root["map"].toArray();
    m_mapData.clear();
    for (const auto& row : mapArr) {
        m_mapData.append(row.toString());
    }

    // 玩家1出生点
    QJsonObject spawn1 = root["playerSpawn"].toObject();
    m_playerSpawn = QPointF(spawn1["x"].toInt() * m_cellSize,
                             spawn1["y"].toInt() * m_cellSize);

    // 玩家2出生点
    if (root.contains("player2Spawn")) {
        QJsonObject spawn2 = root["player2Spawn"].toObject();
        m_player2Spawn = QPointF(spawn2["x"].toInt() * m_cellSize,
                                  spawn2["y"].toInt() * m_cellSize);
    }

    // 基地位置
    if (root.contains("basePosition")) {
        QJsonObject bp = root["basePosition"].toObject();
        m_basePosition = QPointF(bp["x"].toInt() * m_cellSize,
                                  bp["y"].toInt() * m_cellSize);
    }
    if (root.contains("base2Position")) {
        QJsonObject bp2 = root["base2Position"].toObject();
        m_base2Position = QPointF(bp2["x"].toInt() * m_cellSize,
                                   bp2["y"].toInt() * m_cellSize);
    }

    // 敌方坦克配置
    m_enemySpawns.clear();
    m_enemyAITypes.clear();
    m_enemyTankTypes.clear();

    QJsonArray enemies = root["enemies"].toArray();
    for (const auto& enemy : enemies) {
        QJsonObject e = enemy.toObject();
        QJsonObject sp = e["spawn"].toObject();
        m_enemySpawns.append(QPointF(sp["x"].toInt() * m_cellSize,
                                      sp["y"].toInt() * m_cellSize));
        m_enemyAITypes.append(e["ai"].toString("PatrolAI"));

        QString typeStr = e["type"].toString("basic");
        if (typeStr == "fast")       m_enemyTankTypes.append(TankType::EnemyFast);
        else if (typeStr == "heavy") m_enemyTankTypes.append(TankType::EnemyHeavy);
        else if (typeStr == "boss")  m_enemyTankTypes.append(TankType::EnemyBoss);
        else                         m_enemyTankTypes.append(TankType::EnemyBasic);
    }

    // 道具列表
    QJsonArray powerups = root["powerups"].toArray();
    m_powerUpNames.clear();
    for (const auto& p : powerups) {
        m_powerUpNames.append(p.toString());
    }

    qDebug() << "Map: loaded" << m_levelName << "-" << m_mapData.size()
             << "rows," << m_enemySpawns.size() << "enemies,"
             << m_powerUpNames.size() << "powerup types";
    return true;
}

bool Map::loadFromData(const QStringList& mapData) {
    m_mapData = mapData;
    return true;
}

void Map::buildWalls(GameEngine* engine) {
    if (!engine) return;

    for (int r = 0; r < m_mapData.size() && r < m_rows; ++r) {
        const QString& row = m_mapData[r];
        for (int c = 0; c < row.length() && c < m_cols; ++c) {
            QChar cell = row[c];
            QPointF pos(c * m_cellSize, r * m_cellSize);

            switch (cell.toLatin1()) {
            case 'B':
                engine->addWall(Wall(pos, WallType::Brick, m_cellSize));
                break;
            case 'S':
                engine->addWall(Wall(pos, WallType::Steel, m_cellSize));
                break;
            case 'W':
                engine->addWall(Wall(pos, WallType::Water, m_cellSize));
                break;
            default:
                break;  // 空地、出生点标记等
            }
        }
    }
}

char Map::cellAt(int row, int col) const {
    if (row < 0 || row >= m_mapData.size()) return ' ';
    if (col < 0 || col >= m_mapData[row].length()) return ' ';
    return m_mapData[row][col].toLatin1();
}

bool Map::isWallAt(int row, int col) const {
    char c = cellAt(row, col);
    return c == 'B' || c == 'S' || c == 'W';
}
