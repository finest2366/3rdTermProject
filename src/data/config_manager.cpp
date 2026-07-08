/**
 * @file config_manager.cpp
 * @brief 配置管理器实现
 */

#include "config_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

ConfigManager::ConfigManager(const QString& configPath)
    : m_configPath(configPath)
{
    m_config["musicVolume"] = 80;
    m_config["sfxVolume"] = 100;
    m_config["fullscreen"] = false;
}

bool ConfigManager::load() {
    QFile file(m_configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ConfigManager: no config file, using defaults";
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "ConfigManager: invalid JSON";
        return false;
    }
    QJsonObject loaded = doc.object();
    for (auto it = loaded.begin(); it != loaded.end(); ++it) {
        m_config[it.key()] = it.value();
    }
    qDebug() << "ConfigManager: loaded from" << m_configPath;
    return true;
}

bool ConfigManager::save() {
    QFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "ConfigManager: cannot write to" << m_configPath;
        return false;
    }
    QJsonDocument doc(m_config);
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}
