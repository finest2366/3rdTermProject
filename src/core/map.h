/**
 * @file map.h
 * @brief 地图加载与逻辑管理
 *
 * 从 JSON 关卡文件读取地图布局，创建墙壁实体。
 * 地图字符编码：
 *   'B' = 砖墙 (Brick), 'S' = 钢铁墙 (Steel), 'W' = 水域 (Water)
 *   'P' = 玩家出生点标记, 'E' = 敌人出生点标记, ' ' = 空地
 */

#pragma once

#include "game_common.h"
#include <QString>
#include <QVector>
#include <QPointF>
#include <QStringList>

class GameEngine;

class Map {
public:
    Map(int cols, int rows, int cellSize);
    ~Map() = default;

    /** @brief 从 JSON 文件加载完整地图配置 */
    bool loadFromFile(const QString& filePath);

    /** @brief 从字符串列表加载地图布局（程序内使用） */
    bool loadFromData(const QStringList& mapData);

    /** @brief 根据地图数据在引擎中创建墙壁实体 */
    void buildWalls(GameEngine* engine);

    // ---- 属性 ----
    int cols() const { return m_cols; }
    int rows() const { return m_rows; }
    int cellSize() const { return m_cellSize; }
    int width() const { return m_cols * m_cellSize; }
    int height() const { return m_rows * m_cellSize; }

    QPointF playerSpawn() const { return m_playerSpawn; }
    QPointF player2Spawn() const { return m_player2Spawn; }
    QPointF basePosition() const { return m_basePosition; }
    QPointF base2Position() const { return m_base2Position; }

    const QVector<QPointF>& enemySpawns() const { return m_enemySpawns; }
    const QVector<QString>& enemyAITypes() const { return m_enemyAITypes; }
    const QVector<TankType>& enemyTankTypes() const { return m_enemyTankTypes; }
    const QStringList& powerUpNames() const { return m_powerUpNames; }
    const QStringList& mapData() const { return m_mapData; }

    int levelId() const { return m_levelId; }
    const QString& levelName() const { return m_levelName; }
    const QString& levelDescription() const { return m_levelDescription; }

    char cellAt(int row, int col) const;
    bool isWallAt(int row, int col) const;

private:
    int m_cols;
    int m_rows;
    int m_cellSize;
    QStringList m_mapData;

    // 关卡元数据
    int m_levelId = 1;
    QString m_levelName;
    QString m_levelDescription;

    // 出生点/实体配置
    QPointF m_playerSpawn;
    QPointF m_player2Spawn;
    QPointF m_basePosition;
    QPointF m_base2Position;
    QVector<QPointF> m_enemySpawns;
    QVector<QString> m_enemyAITypes;
    QVector<TankType> m_enemyTankTypes;
    QStringList m_powerUpNames;
};
