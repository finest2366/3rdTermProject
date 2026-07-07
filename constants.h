#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>

// 地图尺寸
constexpr int TILE_SIZE = 40;       // 每格像素大小
constexpr int MAP_COLS = 20;        // 地图列数
constexpr int MAP_ROWS = 15;        // 地图行数
constexpr int SCENE_WIDTH = MAP_COLS * TILE_SIZE;   // 800
constexpr int SCENE_HEIGHT = MAP_ROWS * TILE_SIZE;  // 600

// 坦克参数
constexpr int TANK_SIZE = 36;       // 坦克大小 (略小于格子)
constexpr qreal TANK_SPEED = 3.0;   // 坦克移动速度 (像素/帧)
constexpr int TANK_HP = 3;          // 坦克生命值
constexpr int SHOOT_COOLDOWN = 25;  // 射击冷却帧数

// 子弹参数
constexpr int BULLET_SIZE = 8;      // 子弹大小
constexpr qreal BULLET_SPEED = 6.0; // 子弹速度

// 游戏参数
constexpr int GAME_FPS = 60;        // 帧率
constexpr int FRAME_INTERVAL = 1000 / GAME_FPS; // ~16ms
constexpr int ENEMY_COUNT_EASY = 3;
constexpr int ENEMY_COUNT_NORMAL = 5;
constexpr int ENEMY_COUNT_HARD = 8;
constexpr int WIN_SCORE_PVP = 5;    // 双人模式胜利分数
constexpr int BRICK_HP = 3;         // 砖墙生命值（需多次攻击摧毁）
constexpr int BULLET_MAX_BOUNCES = 5; // 子弹最大反弹次数
constexpr qreal INERTIA_FACTOR = 0.5;  // 子弹惯性系数（坦克速度的继承比例）

// 地图元素
enum MapCell {
    CELL_EMPTY = 0,
    CELL_BRICK = 1,     // 砖墙 - 可摧毁
    CELL_STEEL = 2,     // 钢铁 - 不可摧毁
    CELL_WATER = 3,     // 水域 - 坦克不能过，子弹能过
    CELL_GRASS = 4      // 草丛 - 都能过，但遮挡视线
};

// 方向
enum Direction {
    DIR_UP = 0,
    DIR_DOWN = 1,
    DIR_LEFT = 2,
    DIR_RIGHT = 3
};

// 游戏模式
enum GameMode {
    MODE_SINGLE,    // 单人闯关
    MODE_PVP        // 双人对战
};

// 颜色定义
namespace GameColors {
    const QColor PLAYER1_BODY(0x33, 0x99, 0x33);    // 绿色 - 玩家1
    const QColor PLAYER1_BARREL(0x22, 0x66, 0x22);  // 深绿炮管
    const QColor PLAYER2_BODY(0x33, 0x66, 0xCC);    // 蓝色 - 玩家2
    const QColor PLAYER2_BARREL(0x22, 0x44, 0x88);  // 深蓝炮管
    const QColor ENEMY_BODY(0xCC, 0x33, 0x33);      // 红色 - 敌人
    const QColor ENEMY_BARREL(0x88, 0x22, 0x22);    // 深红炮管
    const QColor BULLET(0xFF, 0xDD, 0x00);          // 金色子弹
    const QColor BRICK(0xBB, 0x77, 0x44);           // 棕色砖墙
    const QColor STEEL(0x88, 0x88, 0x88);           // 灰色钢铁
    const QColor WATER(0x33, 0x66, 0xCC);           // 蓝色水域
    const QColor GRASS(0x44, 0x88, 0x22);           // 绿色草丛
    const QColor BASE(0xFF, 0xCC, 0x00);            // 金色基地
    const QColor BACKGROUND(0x22, 0x22, 0x22);      // 深色背景
}

#endif // CONSTANTS_H
