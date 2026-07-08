/**
 * @file settings_dialog.cpp
 * @brief 设置对话框实现
 */

#include "settings_dialog.h"
#include "../data/config_manager.h"
#include "../audio/sound_manager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>

SettingsDialog::SettingsDialog(ConfigManager* config, QWidget* parent)
    : QDialog(parent)
    , m_config(config)
{
    setupUI();
    loadSettings();
    setWindowTitle(QString::fromUtf8("游戏设置"));
    setFixedSize(400, 300);
}

void SettingsDialog::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);

    // 音乐音量
    auto* musicGroup = new QGroupBox(QString::fromUtf8("🎵 音乐音量"), this);
    auto* musicLayout = new QHBoxLayout(musicGroup);
    m_musicSlider = new QSlider(Qt::Horizontal, this);
    m_musicSlider->setRange(0, 100);
    m_musicValueLabel = new QLabel("80", this);
    m_musicValueLabel->setFixedWidth(30);
    musicLayout->addWidget(m_musicSlider);
    musicLayout->addWidget(m_musicValueLabel);
    connect(m_musicSlider, &QSlider::valueChanged, this, [this](int v) {
        m_musicValueLabel->setText(QString::number(v));
    });
    mainLayout->addWidget(musicGroup);

    // 音效音量
    auto* sfxGroup = new QGroupBox(QString::fromUtf8("🔊 音效音量"), this);
    auto* sfxLayout = new QHBoxLayout(sfxGroup);
    m_sfxSlider = new QSlider(Qt::Horizontal, this);
    m_sfxSlider->setRange(0, 100);
    m_sfxValueLabel = new QLabel("100", this);
    m_sfxValueLabel->setFixedWidth(30);
    sfxLayout->addWidget(m_sfxSlider);
    sfxLayout->addWidget(m_sfxValueLabel);
    connect(m_sfxSlider, &QSlider::valueChanged, this, [this](int v) {
        m_sfxValueLabel->setText(QString::number(v));
    });
    mainLayout->addWidget(sfxGroup);

    // 全屏
    m_fullscreenCheck = new QCheckBox(QString::fromUtf8("全屏模式"), this);
    mainLayout->addWidget(m_fullscreenCheck);
    mainLayout->addStretch();

    // 按钮
    auto* btnLayout = new QHBoxLayout();
    auto* applyBtn = new QPushButton(QString::fromUtf8("应用"), this);
    auto* okBtn = new QPushButton(QString::fromUtf8("确定"), this);
    auto* cancelBtn = new QPushButton(QString::fromUtf8("取消"), this);
    connect(applyBtn, &QPushButton::clicked, this, &SettingsDialog::onApply);
    connect(okBtn, &QPushButton::clicked, this, &SettingsDialog::onOk);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(applyBtn);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);
}

void SettingsDialog::loadSettings() {
    m_musicSlider->setValue(m_config->musicVolume());
    m_sfxSlider->setValue(m_config->sfxVolume());
    m_fullscreenCheck->setChecked(m_config->fullscreen());
}

void SettingsDialog::onApply() {
    m_config->setMusicVolume(m_musicSlider->value());
    m_config->setSfxVolume(m_sfxSlider->value());
    m_config->setFullscreen(m_fullscreenCheck->isChecked());

    SoundManager::instance().setMusicVolume(m_musicSlider->value());
    SoundManager::instance().setSfxVolume(m_sfxSlider->value());
    emit fullscreenChanged(m_fullscreenCheck->isChecked());
}

void SettingsDialog::onOk() {
    onApply();
    accept();
}
