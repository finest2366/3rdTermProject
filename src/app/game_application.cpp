/**
 * @file game_application.cpp
 * @brief 游戏应用主类实现
 */

#include "game_application.h"
#include "main_menu.h"
#include "settings_dialog.h"
#include "../ui/game_view.h"
#include "../core/game_engine.h"
#include "../data/config_manager.h"
#include "../data/highscore_manager.h"
#include "../audio/sound_manager.h"
#include "../utils/resource_manager.h"

#include <QMessageBox>
#include <QDebug>

GameApplication::GameApplication(QObject* parent)
    : QObject(parent)
{
    m_config = std::make_unique<ConfigManager>("data/config.json");
    m_highScores = std::make_unique<HighScoreManager>("data/highscores.db");
}

GameApplication::~GameApplication() {
    m_config->save();
}

bool GameApplication::initialize(int argc, char* argv[]) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // 加载用户配置
    m_config->load();

    // 初始化音频
    SoundManager::instance().initialize();
    SoundManager::instance().setMusicVolume(m_config->musicVolume());
    SoundManager::instance().setSfxVolume(m_config->sfxVolume());

    // 初始化高分数据库
    if (!m_highScores->initialize()) {
        qWarning() << "GameApplication: highscore init failed (non-fatal)";
    }

    // 加载自定义素材（如果配置了路径）
    QString customResPath = m_config->customResourcePath();
    if (!customResPath.isEmpty()) {
        ResourceManager::instance().loadResourceConfig(customResPath);
    }

    qDebug() << "GameApplication: initialized successfully";
    return true;
}

void GameApplication::showMainMenu() {
    if (m_gameView) {
        m_gameView->hide();
        m_gameView->deleteLater();
        m_gameView = nullptr;
    }
    if (!m_mainMenu) {
        m_mainMenu = new MainMenu();
        connect(m_mainMenu, &MainMenu::startSingleGame, this, &GameApplication::startSingleGame);
        connect(m_mainMenu, &MainMenu::startMultiGame, this, &GameApplication::startMultiGame);
        connect(m_mainMenu, &MainMenu::settingsRequested, this, &GameApplication::showSettings);
        connect(m_mainMenu, &MainMenu::highScoresRequested, this, &GameApplication::showHighScores);
    }
    m_mainMenu->show();
}

void GameApplication::startSingleGame(int levelId) {
    if (m_mainMenu) m_mainMenu->hide();

    m_gameView = new GameView();
    connect(m_gameView, &GameView::gameFinished, this, &GameApplication::onGameFinished);
    connect(m_gameView, &GameView::backToMenuRequested, this, &GameApplication::showMainMenu);
    m_gameView->startSingleGame(levelId);
    m_gameView->show();
}

void GameApplication::startMultiGame() {
    if (m_mainMenu) m_mainMenu->hide();

    m_gameView = new GameView();
    connect(m_gameView, &GameView::gameFinished, this, &GameApplication::onGameFinished);
    connect(m_gameView, &GameView::backToMenuRequested, this, &GameApplication::showMainMenu);
    m_gameView->startMultiGame();
    m_gameView->show();
}

void GameApplication::showSettings() {
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(m_config.get());
        connect(m_settingsDialog, &SettingsDialog::fullscreenChanged, this, [this](bool fs) {
            QWidget* w = nullptr;
            if (m_gameView && m_gameView->isVisible()) w = m_gameView;
            else if (m_mainMenu && m_mainMenu->isVisible()) w = m_mainMenu;
            if (w) fs ? w->showFullScreen() : w->showNormal();
        });
    }
    m_settingsDialog->exec();
}

void GameApplication::showHighScores() {
    auto scores = m_highScores->getTopScores(10);
    QString text = QString::fromUtf8("=== 高分榜 ===\n\n");
    for (int i = 0; i < scores.size(); ++i) {
        const auto& s = scores[i];
        text += QString("%1. %2  %3  [%4]  %5\n")
            .arg(i + 1, 2).arg(s.player, -12).arg(s.score, 6)
            .arg(s.mode).arg(s.date);
    }
    if (scores.isEmpty()) {
        text += QString::fromUtf8("暂无记录，快去创造历史吧！");
    }
    QMessageBox::information(nullptr, QString::fromUtf8("高分榜"), text);
}

void GameApplication::onGameFinished(int winner) {
    qDebug() << "Game finished, winner:" << winner;

    if (m_gameView && m_gameView->engine()) {
        auto* engine = m_gameView->engine();
        int score = engine->playerScore(0);
        if (score > 0 && !engine->isMultiPlayer()) {
            if (m_highScores->isNewHighScore(score, "single")) {
                m_highScores->addScore("Player1", score, "single");
            }
        }
    }
    showMainMenu();
}
