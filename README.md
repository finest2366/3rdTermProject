# 坦克大战 (TankBattle)

经典坦克大战游戏，基于 **Qt 6** + **C++17** 开发。

## 游戏特性

- **单人闯关模式** — 对抗 AI 敌方坦克，守护基地
- **双人对战模式 (PVP)** — 两名玩家同屏对战
- **多种地形** — 砖墙（可摧毁）、钢铁（不可摧毁）、水域（阻挡坦克、子弹穿过）、草丛（遮挡视野）
- **子弹弹射** — 子弹碰到钢铁墙壁会反弹，继承坦克惯性
- **三种难度** — 简单 (3 敌) / 普通 (5 敌) / 困难 (8 敌)

## 技术参数

| 项目 | 详情 |
|------|------|
| 语言 | C++17 |
| 框架 | Qt 6 (Core / Gui / Widgets) |
| 构建 | CMake 3.22+ |
| 包管理 | vcpkg |
| 编译器 | MSYS2 UCRT64 MinGW (GCC) |
| 分辨率 | 800×600 (20×15 格, 每格 40px) |
| 帧率 | 60 FPS |

## 快速开始

### 环境要求

- [MSYS2](https://www.msys2.org/)（UCRT64 环境）
- [vcpkg](https://github.com/microsoft/vcpkg)（配置好 `VCPKG_ROOT` 环境变量）

### 一键构建运行

双击 `build.bat`，脚本会自动：

1. 检测或自动安装 MSYS2 UCRT64 工具链
2. 通过 vcpkg 安装 Qt6 依赖
3. CMake 配置 → 编译 → 自动部署 Qt DLL → 启动游戏

### 手动构建

```bash
# 在 MSYS2 UCRT64 终端中
cmake -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build -j$(nproc)
```

## 项目结构

```
├── main.cpp              # 入口
├── mainwindow.h/cpp      # 主窗口（模式选择、分数管理）
├── startscene.h/cpp      # 开始界面
├── gamescene.h/cpp       # 游戏场景（核心渲染与碰撞）
├── gamemanager.h/cpp     # 游戏管理器（AI、波次控制）
├── tank.h/cpp            # 坦克基类
├── playertank.h/cpp      # 玩家坦克
├── enemytank.h/cpp       # AI 敌方坦克
├── bullet.h/cpp          # 子弹（弹射、惯性）
├── wall.h/cpp            # 墙壁（砖墙/钢铁）
├── base.h/cpp            # 基地
├── constants.h           # 游戏常量与枚举
├── mapdata.h             # 地图数据
├── build.bat             # 一键构建引导脚本
├── CMakeLists.txt        # CMake 配置
└── data/                 # 游戏数据（高分记录等）
```

## 操作方式

| 按键 | 玩家 1 | 玩家 2 |
|------|--------|--------|
| 移动 | W/A/S/D | ↑/←/↓/→ |
| 射击 | F | L |

## 版本历史

- **v0.04** — 迁移到 CMake + vcpkg 构建系统，清理二进制依赖
- **v0.03** — 修复编译错误与游戏卡顿
- **v0.02** — 装甲核心：休闲坦克大战 v2.0
- **v0.01** — 坦克大战项目初版
