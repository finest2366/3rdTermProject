/**
 * @file main.cpp
 * @brief 装甲核心：休闲坦克大战 v2.0 — 程序入口
 *
 * 启动流程：
 *   1. 初始化 QApplication（全局样式、中文字体）
 *   2. 加载配置 (config.json)
 *   3. 初始化音频系统 + 高分数据库
 *   4. 显示主菜单
 *   5. 进入事件循环
 */

#include <QApplication>
#include <QDebug>
#include "app/game_application.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8("装甲核心：休闲坦克大战"));
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("TankGameStudio");

    // 全局样式
    app.setStyleSheet(R"(
        QWidget {
            font-family: "Microsoft YaHei", "Arial", sans-serif;
        }
        QPushButton {
            padding: 8px 16px;
            border: 2px solid #555;
            border-radius: 6px;
            background-color: #333;
            color: white;
            font-size: 14px;
            min-width: 120px;
        }
        QPushButton:hover { background-color: #555; }
        QPushButton:pressed { background-color: #222; }
        QSlider::groove:horizontal {
            height: 6px;
            background: #555;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            width: 18px;
            height: 18px;
            margin: -6px 0;
            background: #e94560;
            border-radius: 9px;
        }
    )");

    // 初始化
    GameApplication gameApp;
    if (!gameApp.initialize(argc, argv)) {
        qCritical() << "Failed to initialize game application";
        return -1;
    }

    gameApp.showMainMenu();
    return app.exec();
}
