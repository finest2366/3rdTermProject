/**
 * @file resource_manager.cpp
 * @brief 资源管理器实现
 */

#include "resource_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ResourceManager& ResourceManager::instance() {
    static ResourceManager inst;
    return inst;
}

QPixmap ResourceManager::getImage(const QString& key) const {
    if (m_pixmapCache.contains(key)) {
        return m_pixmapCache[key];
    }
    if (!m_imageMap.contains(key)) {
        qWarning() << "ResourceManager: key not found:" << key;
        return QPixmap();
    }
    QString path = m_imageMap[key];
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "ResourceManager: failed to load:" << path;
    } else {
        m_pixmapCache[key] = pixmap;
    }
    return pixmap;
}

bool ResourceManager::loadResourceConfig(const QString& configPath) {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ResourceManager: cannot open:" << configPath;
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "ResourceManager: invalid JSON";
        return false;
    }
    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        // 跳过注释字段
        if (it.key().startsWith('_')) continue;
        m_imageMap[it.key()] = it.value().toString();
        m_pixmapCache.remove(it.key());
        qDebug() << "ResourceManager:" << it.key() << "→" << it.value().toString();
    }
    return true;
}

void ResourceManager::registerDefaultImage(const QString& key, const QString& resourcePath) {
    m_imageMap[key] = resourcePath;
}

void ResourceManager::registerImage(const QString& key, const QString& path) {
    m_imageMap[key] = path;
    m_pixmapCache.remove(key);
}

QStringList ResourceManager::registeredKeys() const {
    return m_imageMap.keys();
}
