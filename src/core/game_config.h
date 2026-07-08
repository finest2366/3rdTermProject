/**
 * @file game_config.h
 * @brief 游戏统一配置接口 —— 所有可调参数集中于此
 *
 * 修改此文件即可调整游戏难度、平衡性、地图大小等。
 * 所有数值均为编译时常量，修改后需重新编译。
 *
 * 分类索引：
 *   1. 地图与显示
 *   2. 玩家坦克
 *   3. 敌方坦克
 *   4. 子弹
 *   5. AI 行为
 *   6. 道具系统
 *   7. 技能系统
 *   8. 计分与胜负
 */

#pragma once

namespace GameConfig {

// ============================================================
// 1. 地图与显示
// ============================================================
constexpr int MAP_COLS = 15;                // 地图网格列数
constexpr int MAP_ROWS = 15;                // 地图网格行数
constexpr int CELL_SIZE = 40;               // 每格像素大小
constexpr int TANK_SIZE = 36;               // 坦克像素大小（略小于格子）
constexpr int POWERUP_SIZE = 30;            // 道具图标像素大小
constexpr int GAME_FPS = 60;                // 刷新率
constexpr int FRAME_MS = 1000 / GAME_FPS;   // 每帧毫秒数

// ============================================================
// 2. 玩家坦克
// ============================================================
constexpr int PLAYER1_MAX_HP = 3;
constexpr double PLAYER1_SPEED = 3.0;
constexpr int PLAYER1_FIRE_COOLDOWN_MS = 400;

constexpr int PLAYER2_MAX_HP = 3;
constexpr double PLAYER2_SPEED = 3.0;
constexpr int PLAYER2_FIRE_COOLDOWN_MS = 400;

constexpr int PLAYER_SINGLE_LIVES = 3;      // 单人模式初始生命数
constexpr int PLAYER_MULTI_LIVES = 3;       // 双人模式每人初始生命数

// ============================================================
// 3. 敌方坦克
// ============================================================

// --- 普通敌人 (basic) ---
constexpr int ENEMY_BASIC_HP = 1;
constexpr double ENEMY_BASIC_SPEED = 1.5;
constexpr int ENEMY_BASIC_FIRE_COOLDOWN_MS = 800;

// --- 快速敌人 (fast) ---
constexpr int ENEMY_FAST_HP = 1;
constexpr double ENEMY_FAST_SPEED = 2.5;
constexpr int ENEMY_FAST_FIRE_COOLDOWN_MS = 700;

// --- 重装敌人 (heavy) ---
constexpr int ENEMY_HEAVY_HP = 3;
constexpr double ENEMY_HEAVY_SPEED = 1.0;
constexpr int ENEMY_HEAVY_FIRE_COOLDOWN_MS = 1000;

// --- Boss 敌人 (新增) ---
constexpr int ENEMY_BOSS_HP = 6;
constexpr double ENEMY_BOSS_SPEED = 0.8;
constexpr int ENEMY_BOSS_FIRE_COOLDOWN_MS = 600;

// ============================================================
// 4. 子弹
// ============================================================
constexpr double BULLET_SPEED = 5.0;        // 子弹速度（像素/帧）
constexpr double BULLET_RADIUS = 3.0;       // 子弹半径
constexpr double BULLET_SIZE = BULLET_RADIUS * 2.0;

// ============================================================
// 5. AI 行为
// ============================================================
constexpr int AI_FIRE_PROBABILITY_PERCENT = 3;   // 默认开火概率（备用）
constexpr double PATROL_AI_FIRE_RANGE = 300.0;    // 巡逻AI射程
constexpr double PATROL_AI_ALIGN_THRESHOLD = 40.0;
constexpr int PATROL_AI_MAX_STEPS_PER_DIR = 60;
constexpr int PATROL_AI_FORCE_TURN_STEPS = 200;

constexpr double AGGRESSIVE_AI_FIRE_RANGE = 350.0;
constexpr double AGGRESSIVE_AI_ALIGN_THRESHOLD = 40.0;
constexpr int AGGRESSIVE_AI_RANDOM_TURN_STEPS = 100;

// Boss AI 参数
constexpr double BOSS_AI_FIRE_RANGE = 400.0;
constexpr int BOSS_AI_BURST_COUNT = 3;           // 每次连发子弹数
constexpr int BOSS_MIN_HP_FOR_FLEE = 2;          // 低于此HP开始躲避

// ============================================================
// 6. 道具系统
// ============================================================
constexpr double SPEED_BOOST_MULTIPLIER = 1.8;
constexpr int SPEED_BOOST_DURATION_MS = 8000;

constexpr int RAPID_FIRE_COOLDOWN_DIVISOR = 3;
constexpr int RAPID_FIRE_DURATION_MS = 6000;

constexpr int HEAL_AMOUNT = 1;

constexpr int FREEZE_DURATION_MS = 5000;         // 冰冻持续5秒
constexpr int INVISIBLE_DURATION_MS = 6000;      // 隐身持续6秒

// ============================================================
// 7. 技能系统
// ============================================================
constexpr int SKILL_SPRINT_COOLDOWN_MS = 10000;   // 疾跑冷却10秒
constexpr int SKILL_SPRINT_DURATION_MS = 3000;    // 疾跑持续3秒
constexpr double SKILL_SPRINT_SPEED_MULT = 2.5;

constexpr int SKILL_MISSILE_COOLDOWN_MS = 15000;  // 导弹冷却15秒

constexpr int SKILL_SHIELD_WALL_COOLDOWN_MS = 20000; // 盾墙冷却20秒

// ============================================================
// 8. 计分与胜负
// ============================================================
constexpr int SCORE_PER_KILL = 100;
constexpr int SCORE_BOSS_KILL = 500;             // Boss 击杀额外加分
constexpr int SCORE_POWERUP_COLLECT = 20;        // 拾取道具加分
constexpr bool BASE_DESTROY_ENDS_GAME = true;

} // namespace GameConfig
