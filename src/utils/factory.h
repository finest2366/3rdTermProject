/**
 * @file factory.h
 * @brief 对象工厂 —— 道具和 AI 控制器的自动注册与创建
 *
 * 使用 REGISTER_POWERUP / REGISTER_AI 宏在类实现文件中自动注册。
 * 工厂通过静态单例维护注册表，运行时根据名称创建实例。
 *
 * 扩展新道具：
 *   1. 继承 PowerUp 实现 apply()
 *   2. 在 .cpp 底部添加 REGISTER_POWERUP(MyPowerUp)
 *   3. 在关卡 JSON 的 powerups 数组中使用 "MyPowerUp"
 *
 * 扩展新 AI：
 *   1. 继承 AIController 实现 update()
 *   2. 在 .cpp 底部添加 REGISTER_AI(MyAI)
 *   3. 在关卡 JSON 中将敌人的 ai 字段设置为 "MyAI"
 */

#pragma once

#include <QMap>
#include <QString>
#include <functional>
#include <memory>

class PowerUp;
class Tank;
class AIController;

// ============================================================
// 道具工厂
// ============================================================
class PowerUpFactory {
public:
    using CreatorFunc = std::function<PowerUp*()>;
    static PowerUpFactory& instance();

    bool registerType(const QString& name, CreatorFunc creator);
    PowerUp* create(const QString& name) const;
    QStringList registeredTypes() const;

private:
    PowerUpFactory() = default;
    QMap<QString, CreatorFunc> m_registry;
};

// ============================================================
// AI 控制器工厂
// ============================================================
class AIControllerFactory {
public:
    using CreatorFunc = std::function<AIController*()>;
    static AIControllerFactory& instance();

    bool registerType(const QString& name, CreatorFunc creator);
    AIController* create(const QString& name) const;

private:
    AIControllerFactory() = default;
    QMap<QString, CreatorFunc> m_registry;
};

// ============================================================
// 注册宏
// ============================================================
#define REGISTER_POWERUP(ClassName) \
    static bool _reg_powerup_##ClassName = \
        PowerUpFactory::instance().registerType(#ClassName, []() -> PowerUp* { return new ClassName(); })

#define REGISTER_AI(ClassName) \
    static bool _reg_ai_##ClassName = \
        AIControllerFactory::instance().registerType(#ClassName, []() -> AIController* { return new ClassName(); })
