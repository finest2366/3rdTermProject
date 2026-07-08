/**
 * @file settings_dialog.h
 * @brief 设置对话框 —— 调整音效/音乐音量、全屏模式
 */

#pragma once

#include <QDialog>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>

class ConfigManager;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(ConfigManager* config, QWidget* parent = nullptr);
    ~SettingsDialog() override = default;

signals:
    void fullscreenChanged(bool fullscreen);

private slots:
    void onApply();
    void onOk();

private:
    void setupUI();
    void loadSettings();

    ConfigManager* m_config;
    QSlider* m_musicSlider = nullptr;
    QSlider* m_sfxSlider = nullptr;
    QCheckBox* m_fullscreenCheck = nullptr;
    QLabel* m_musicValueLabel = nullptr;
    QLabel* m_sfxValueLabel = nullptr;
};
