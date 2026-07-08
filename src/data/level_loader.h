/**
 * @file level_loader.h
 * @brief 关卡加载器 —— 从 JSON 文件读取关卡配置
 */

#pragma once

#include <QString>
#include <QJsonObject>
#include <QVector>

class Map;

class LevelLoader {
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    QJsonObject loadLevel(int levelId) const;
    int levelCount() const;
    QString levelName(int levelId) const;
    static bool parseLevel(const QJsonObject& json, Map& map);

private:
    QString levelFilePath(int levelId) const;
};
