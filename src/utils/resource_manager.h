/**
 * @file resource_manager.h
 * @brief 资源管理器 —— 管理游戏所有图像/音频资源的加载与替换
 *
 * ## 素材替换接口（用户文档）
 *
 * ### 如何替换游戏素材？
 *
 * 1. **准备素材文件**：将你的 PNG 图片放在任意目录下
 * 2. **编辑配置文件**：修改 `data/custom_resources.json`
 * 3. **重启游戏**：新素材自动加载
 *
 * ### 支持的素材 Key 列表
 *
 * | Key | 说明 | 默认尺寸 |
 * |-----|------|----------|
 * | `tank_player1` | 玩家1坦克 | 36x36 |
 * | `tank_player2` | 玩家2坦克 | 36x36 |
 * | `enemy_basic` | 普通敌人 | 36x36 |
 * | `enemy_fast` | 快速敌人 | 36x36 |
 * | `enemy_heavy` | 重装敌人 | 36x36 |
 * | `enemy_boss` | Boss | 48x48 |
 * | `wall_brick` | 砖墙 | 40x40 |
 * | `wall_steel` | 钢铁墙 | 40x40 |
 * | `wall_water` | 水域 | 40x40 |
 * | `bullet_player` | 玩家子弹 | 6x6 |
 * | `bullet_enemy` | 敌方子弹 | 6x6 |
 * | `base` | 基地 | 40x40 |
 * | `powerup_Speed Boost` | 加速道具 | 30x30 |
 * | `powerup_Shield` | 护盾道具 | 30x30 |
 * | `powerup_Rapid Fire` | 连射道具 | 30x30 |
 * | `powerup_Heal` | 回血道具 | 30x30 |
 * | `powerup_Bomb` | 全屏炸弹 | 30x30 |
 * | `powerup_Freeze` | 冰冻道具 | 30x30 |
 * | `powerup_Invisible` | 隐身道具 | 30x30 |
 * | `skill_Sprint` | 疾跑技能图标 | 32x32 |
 * | `skill_Missile` | 导弹技能图标 | 32x32 |
 * | `skill_Shield Wall` | 盾墙技能图标 | 32x32 |
 * | `background` | 背景图 | 600x600 |
 *
 * ### custom_resources.json 示例
 *
 * ```json
 * {
 *   "tank_player1": "./my_skins/tank_red.png",
 *   "wall_brick": "./my_skins/brick_gold.png",
 *   "enemy_boss": "./my_skins/boss_dragon.png"
 * }
 * ```
 */

#pragma once

#include <QMap>
#include <QString>
#include <QPixmap>
#include <QJsonObject>

class ResourceManager {
public:
    static ResourceManager& instance();

    /** @brief 根据 key 获取图像（带缓存） */
    QPixmap getImage(const QString& key) const;

    /** @brief 从 JSON 配置文件加载素材映射 */
    bool loadResourceConfig(const QString& configPath);

    /** @brief 注册默认素材路径（通常来自 .qrc 或 assets/） */
    void registerDefaultImage(const QString& key, const QString& resourcePath);

    /** @brief 手动注册/覆盖单个素材路径 */
    void registerImage(const QString& key, const QString& path);

    /** @brief 获取所有已注册的素材 key */
    QStringList registeredKeys() const;

private:
    ResourceManager() = default;
    QMap<QString, QString> m_imageMap;
    mutable QMap<QString, QPixmap> m_pixmapCache;
};
