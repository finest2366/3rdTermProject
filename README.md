# 装甲核心：休闲坦克大战 v2.0 (Armor Core: Casual Tank Battle)

经典坦克大战规则的休闲射击游戏，支持单人闯关和双人对战。

## v2.0 新增功能

- 🧠 **AI决策树框架**：正式的行为树，可组合决策节点
- 🤖 **Boss敌人**：6HP、连发射击、低血量逃跑策略
- 💊 **7种道具**：加速/护盾/连射/回血/炸弹/冰冻/隐身
- ⚡ **3种技能**：疾跑/导弹/盾墙，Q键激活
- 🌊 **水域地形**：阻挡移动但子弹可通过
- ❄️ **冰冻系统**：冻结所有敌人
- 👻 **隐身系统**：敌人无法瞄准
- 🎵 **完整音频**：BGM循环、音效播放
- 📊 **5个关卡**：渐进式难度设计

## 构建要求

- C++17 或更高版本
- Qt 6 (Widgets, Multimedia, Sql)
- CMake 3.16+

## 构建步骤

```bash
# 使用 CMakePresets (推荐)
cmake --preset Qt-MinGW-Debug
cmake --build out/build/Qt-MinGW-Debug

# 或手动配置
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build .
```

## 键位说明

### 单人模式
| 操作 | 按键 |
|------|------|
| 移动 | W A S D |
| 射击 | 空格 |
| 技能 | Q |
| 暂停 | ESC |

### 双人模式
| 操作 | 玩家1 | 玩家2 |
|------|-------|-------|
| 移动 | W A S D | ↑ ← ↓ → |
| 射击 | 空格 | 回车 |
| 技能 | Q | Shift |
| 暂停 | ESC | ESC |

---

## 素材替换接口

### 快速开始

编辑 `data/custom_resources.json`，将素材 key 映射到你的图片路径：

```json
{
  "tank_player1": "./my_skins/tank_red.png",
  "wall_brick": "./my_skins/brick_gold.png",
  "enemy_boss": "./my_skins/boss_dragon.png"
}
```

### 完整素材 Key 列表

| Key | 说明 | 尺寸 |
|-----|------|------|
| `tank_player1` | 玩家1坦克 | 36×36 |
| `tank_player2` | 玩家2坦克 | 36×36 |
| `enemy_basic` | 普通敌人 | 36×36 |
| `enemy_fast` | 快速敌人 | 36×36 |
| `enemy_heavy` | 重装敌人 | 36×36 |
| `enemy_boss` | Boss | 48×48 |
| `wall_brick` | 砖墙 | 40×40 |
| `wall_steel` | 钢铁墙 | 40×40 |
| `wall_water` | 水域 | 40×40 |
| `bullet_player` | 玩家子弹 | 6×6 |
| `bullet_enemy` | 敌方子弹 | 6×6 |
| `base` | 基地 | 40×40 |
| `powerup_Speed Boost` | 加速道具 | 30×30 |
| `powerup_Shield` | 护盾道具 | 30×30 |
| `powerup_Rapid Fire` | 连射道具 | 30×30 |
| `powerup_Heal` | 回血道具 | 30×30 |
| `powerup_Bomb` | 炸弹道具 | 30×30 |
| `powerup_Freeze` | 冰冻道具 | 30×30 |
| `powerup_Invisible` | 隐身道具 | 30×30 |
| `skill_Sprint` | 疾跑技能 | 32×32 |
| `skill_Missile` | 导弹技能 | 32×32 |
| `skill_Shield Wall` | 盾墙技能 | 32×32 |
| `background` | 背景图 | 600×600 |

### 音效文件

将 .wav 文件放入 `assets/sounds/` 目录：

| 文件 | 说明 |
|------|------|
| `assets/sounds/shoot.wav` | 射击 |
| `assets/sounds/explosion.wav` | 爆炸 |
| `assets/sounds/powerup.wav` | 拾取道具 |
| `assets/sounds/freeze.wav` | 冰冻 |
| `assets/sounds/bgm.wav` | 背景音乐 |

### 代码级素材接口

```cpp
// 通过 ResourceManager 单例访问
#include "utils/resource_manager.h"

// 获取素材
QPixmap img = ResourceManager::instance().getImage("tank_player1");

// 动态注册素材
ResourceManager::instance().registerImage("custom_key", "./path/to/image.png");

// 从 JSON 批量加载
ResourceManager::instance().loadResourceConfig("data/custom_resources.json");
```

---

## 项目结构

```
TankGameMerged/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── assets/                   # 美术素材（放入图片/音效）
│   ├── images/powerups/
│   └── sounds/
├── data/                     # 运行时数据
│   ├── config.json           # 用户设置
│   ├── custom_resources.json # 素材替换配置 ★
│   └── levels/               # 关卡 JSON
├── src/
│   ├── main.cpp
│   ├── core/   # 游戏核心引擎
│   ├── ai/     # AI + 决策树
│   ├── ui/     # Qt 图形渲染
│   ├── audio/  # 音效管理
│   ├── data/   # 数据存取
│   ├── utils/  # 资源管理 + 工厂
│   └── app/    # 菜单 + 设置
└── tests/      # 单元测试
```

## 扩展指南

### 添加新道具
1. 在 `src/core/powerup.h` 中添加新类继承 `PowerUp`
2. 实现 `apply(Tank*)` 方法
3. 在 `.cpp` 中使用 `REGISTER_POWERUP(ClassName)` 注册
4. 在关卡 JSON 的 `powerups` 数组中添加类名

### 添加新 AI
1. 在 `src/ai/` 中添加新类继承 `AIController`
2. 实现 `update(EnemyTank*, const GameState&)` 方法
3. 使用 `REGISTER_AI(ClassName)` 注册
4. 在关卡 JSON 中将敌人的 `ai` 字段设为新 AI 名称

### 添加新决策节点
1. 在 `src/ai/decision_tree.h` 中添加新类继承 `DecisionNode`
2. 实现 `evaluate()` 方法
3. 在 AI 构造函数中通过 `addChild()` 加入决策链

## License

MIT License
