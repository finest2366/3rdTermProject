/**
 * @file sound_manager.h
 * @brief 音频管理器 —— 管理游戏所有音效和背景音乐
 *
 * 素材接口：音效文件路径映射在 soundFilePath() 中定义。
 * 替换音效：将对应的 .wav 文件放入 assets/sounds/ 目录即可。
 * 支持的音效文件：
 *   assets/sounds/shoot.wav      - 射击
 *   assets/sounds/explosion.wav  - 爆炸
 *   assets/sounds/powerup.wav    - 拾取道具
 *   assets/sounds/freeze.wav     - 冰冻
 *   assets/sounds/bgm.wav        - 背景音乐
 */

#pragma once

#include <QObject>
#include "../core/game_common.h"

class QMediaPlayer;
class QAudioOutput;

class SoundManager : public QObject {
    Q_OBJECT
public:
    static SoundManager& instance();

    bool initialize();
    void playSound(SoundId id);
    void playBGM();
    void stopBGM();

    void setMusicVolume(int vol);
    void setSfxVolume(int vol);

    int musicVolume() const { return m_musicVolume; }
    int sfxVolume() const { return m_sfxVolume; }

private:
    SoundManager();
    ~SoundManager() override;

    QString soundFilePath(SoundId id) const;

    int m_musicVolume = 80;
    int m_sfxVolume = 100;
    bool m_initialized = false;

    QMediaPlayer* m_bgmPlayer = nullptr;
    QAudioOutput* m_bgmOutput = nullptr;
};
