/**
 * @file level_loader.cpp
 * @brief 关卡加载器实现
 */

#include "level_loader.h"
#include "../core/map.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

QJsonObject LevelLoader::loadLevel(int levelId) const {
    QString path = levelFilePath(levelId);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "LevelLoader: cannot open" << path;
        return QJsonObject();
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "LevelLoader: invalid JSON in" << path;
        return QJsonObject();
    }
    return doc.object();
}

int LevelLoader::levelCount() const {
    QDir dir("data/levels");
    QStringList filters;
    filters << "level_*.json";
    return static_cast<int>(dir.entryList(filters, QDir::Files).size());
}

QString LevelLoader::levelName(int levelId) const {
    QJsonObject json = loadLevel(levelId);
    if (json.isEmpty()) return QString("Level %1").arg(levelId);
    return json["name"].toString(QString("Level %1").arg(levelId));
}

bool LevelLoader::parseLevel(const QJsonObject& json, Map& map) {
    Q_UNUSED(json);
    Q_UNUSED(map);
    return true;  // 实际解析在 Map::loadFromFile 中
}

QString LevelLoader::levelFilePath(int levelId) const {
    return QString("data/levels/level_%1.json").arg(levelId, 2, 10, QChar('0'));
}
