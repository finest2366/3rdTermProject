/**
 * @file sound_manager.cpp
 * @brief 音频管理器实现 —— 使用 Qt Multimedia
 *
 * 素材接口说明：
 *   默认音效文件路径在 soundFilePath() 方法中定义。
 *   用户替换音效：将同名 .wav 文件放入 assets/sounds/ 目录。
 *   也可通过 ResourceManager 的音效注册接口动态替换路径。
 */

#include "sound_manager.h"
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QUrl>

SoundManager& SoundManager::instance() {
    static SoundManager inst;
    return inst;
}

SoundManager::SoundManager() {}
SoundManager::~SoundManager() {
    stopBGM();
}

bool SoundManager::initialize() {
    // 初始化 BGM 播放器
    m_bgmOutput = new QAudioOutput(this);
    m_bgmOutput->setVolume(m_musicVolume / 100.0);

    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmPlayer->setAudioOutput(m_bgmOutput);
    m_bgmPlayer->setLoops(QMediaPlayer::Infinite);  // BGM 循环

    m_initialized = true;
    qDebug() << "SoundManager: initialized";
    return true;
}

void SoundManager::playSound(SoundId id) {
    if (!m_initialized || m_sfxVolume <= 0) return;

    QString path = soundFilePath(id);
    if (path.isEmpty()) {
        qDebug() << "SoundManager: no sound file for" << static_cast<int>(id);
        return;
    }

    auto* effect = new QSoundEffect(this);
    // 尝试从文件系统加载，不存在则静默
    QUrl url = QUrl::fromLocalFile(path);
    effect->setSource(url);
    effect->setVolume(m_sfxVolume / 100.0);
    effect->play();

    connect(effect, &QSoundEffect::playingChanged, effect, [effect]() {
        if (!effect->isPlaying()) {
            effect->deleteLater();
        }
    });
}

void SoundManager::playBGM() {
    if (!m_initialized || !m_bgmPlayer) return;
    QString path = soundFilePath(SoundId::BGM);
    if (!path.isEmpty()) {
        m_bgmPlayer->setSource(QUrl::fromLocalFile(path));
        m_bgmPlayer->play();
    }
    qDebug() << "SoundManager: BGM started";
}

void SoundManager::stopBGM() {
    if (m_bgmPlayer) {
        m_bgmPlayer->stop();
    }
}

void SoundManager::setMusicVolume(int vol) {
    m_musicVolume = qBound(0, vol, 100);
    if (m_bgmOutput) {
        m_bgmOutput->setVolume(m_musicVolume / 100.0);
    }
}

void SoundManager::setSfxVolume(int vol) {
    m_sfxVolume = qBound(0, vol, 100);
}

QString SoundManager::soundFilePath(SoundId id) const {
    // 素材路径接口 —— 修改这里即可更换默认音效路径
    switch (id) {
    case SoundId::Shoot:      return "assets/sounds/shoot.wav";
    case SoundId::Explosion:  return "assets/sounds/explosion.wav";
    case SoundId::PowerUp:    return "assets/sounds/powerup.wav";
    case SoundId::BGM:        return "assets/sounds/bgm.wav";
    case SoundId::Freeze:     return "assets/sounds/freeze.wav";
    case SoundId::Invisible:  return "assets/sounds/powerup.wav";
    case SoundId::MenuClick:  return "assets/sounds/shoot.wav";
    case SoundId::GameOver:   return "assets/sounds/explosion.wav";
    default: return QString();
    }
}
