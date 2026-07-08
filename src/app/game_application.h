/**
 * @file game_application.h
 * @brief 游戏应用程序主类 —— 管理应用生命周期和窗口切换
 */

#pragma once

#include <QObject>
#include <memory>

class ConfigManager;
class HighScoreManager;
class MainMenu;
class GameView;
class SettingsDialog;

class GameApplication : public QObject {
    Q_OBJECT
public:
    explicit GameApplication(QObject* parent = nullptr);
    ~GameApplication() override;

    bool initialize(int argc, char* argv[]);
    void showMainMenu();
    void startSingleGame(int levelId);
    void startMultiGame();
    void showSettings();
    void showHighScores();

    ConfigManager* config() const { return m_config.get(); }
    HighScoreManager* highScores() const { return m_highScores.get(); }

private slots:
    void onGameFinished(int winner);

private:
    std::unique_ptr<ConfigManager> m_config;
    std::unique_ptr<HighScoreManager> m_highScores;

    MainMenu* m_mainMenu = nullptr;
    GameView* m_gameView = nullptr;
    SettingsDialog* m_settingsDialog = nullptr;
};
