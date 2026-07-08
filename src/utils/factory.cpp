/**
 * @file factory.cpp
 * @brief 工厂实现
 */

#include "factory.h"
#include <QDebug>

// ===== PowerUpFactory =====
PowerUpFactory& PowerUpFactory::instance() {
    static PowerUpFactory inst;
    return inst;
}

bool PowerUpFactory::registerType(const QString& name, CreatorFunc creator) {
    if (m_registry.contains(name)) {
        qWarning() << "PowerUpFactory: duplicate:" << name;
        return false;
    }
    m_registry[name] = creator;
    qDebug() << "PowerUpFactory: registered" << name;
    return true;
}

PowerUp* PowerUpFactory::create(const QString& name) const {
    if (!m_registry.contains(name)) {
        qWarning() << "PowerUpFactory: unknown:" << name;
        return nullptr;
    }
    return m_registry[name]();
}

QStringList PowerUpFactory::registeredTypes() const {
    return m_registry.keys();
}

// ===== AIControllerFactory =====
AIControllerFactory& AIControllerFactory::instance() {
    static AIControllerFactory inst;
    return inst;
}

bool AIControllerFactory::registerType(const QString& name, CreatorFunc creator) {
    if (m_registry.contains(name)) {
        qWarning() << "AIControllerFactory: duplicate:" << name;
        return false;
    }
    m_registry[name] = creator;
    qDebug() << "AIControllerFactory: registered" << name;
    return true;
}

AIController* AIControllerFactory::create(const QString& name) const {
    if (!m_registry.contains(name)) {
        qWarning() << "AIControllerFactory: unknown:" << name;
        return nullptr;
    }
    return m_registry[name]();
}
