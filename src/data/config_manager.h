/**
 * @file config_manager.h
 * @brief 配置管理器 —— 读写用户设置 JSON 文件
 */

#pragma once

#include <QString>
#include <QJsonObject>

class ConfigManager {
public:
    explicit ConfigManager(const QString& configPath = "data/config.json");
    ~ConfigManager() = default;

    bool load();
    bool save();

    int musicVolume() const { return m_config["musicVolume"].toInt(80); }
    void setMusicVolume(int vol) { m_config["musicVolume"] = vol; }
    int sfxVolume() const { return m_config["sfxVolume"].toInt(100); }
    void setSfxVolume(int vol) { m_config["sfxVolume"] = vol; }
    bool fullscreen() const { return m_config["fullscreen"].toBool(false); }
    void setFullscreen(bool fs) { m_config["fullscreen"] = fs; }
    QString customResourcePath() const { return m_config["customResourcePath"].toString(""); }
    void setCustomResourcePath(const QString& path) { m_config["customResourcePath"] = path; }

    const QJsonObject& config() const { return m_config; }

private:
    QString m_configPath;
    QJsonObject m_config;
};
