/**
 * @file hud.cpp
 * @brief HUD 实现 —— 包含技能冷却显示
 */

#include "hud.h"
#include "../core/game_engine.h"
#include "../core/tank.h"
#include "../core/skill.h"

HUD::HUD(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(80);
    setStyleSheet(
        "background-color: rgba(0, 0, 0, 180);"
        "color: white;"
        "font-family: 'Microsoft YaHei', Arial;"
        "font-size: 13px;"
    );

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);

    // --- 玩家1 ---
    auto* p1Layout = new QVBoxLayout();
    m_player1Label = new QLabel(QStringLiteral("P1"), this);
    m_player1Label->setStyleSheet("color: #00C800; font-weight: bold; font-size: 15px;");
    m_player1Lives = new QLabel(QStringLiteral("HP: ♥♥♥"), this);
    m_player1Score = new QLabel(QStringLiteral("Score: 0"), this);
    m_player1Skill = new QLabel(QStringLiteral("Skill: Ready"), this);
    m_player1Skill->setStyleSheet("color: #aaa; font-size: 11px;");
    p1Layout->addWidget(m_player1Label);
    p1Layout->addWidget(m_player1Lives);
    p1Layout->addWidget(m_player1Score);
    p1Layout->addWidget(m_player1Skill);

    // --- 游戏状态 ---
    auto* statusLayout = new QVBoxLayout();
    m_levelInfo = new QLabel("", this);
    m_levelInfo->setStyleSheet("color: #ccc; font-size: 12px;");
    m_levelInfo->setAlignment(Qt::AlignCenter);
    m_gameStatus = new QLabel("FIGHT!", this);
    m_gameStatus->setStyleSheet("color: yellow; font-size: 18px; font-weight: bold;");
    m_gameStatus->setAlignment(Qt::AlignCenter);
    m_enemyCount = new QLabel("Enemies: 0/0", this);
    m_enemyCount->setAlignment(Qt::AlignCenter);
    statusLayout->addWidget(m_levelInfo);
    statusLayout->addWidget(m_gameStatus);
    statusLayout->addWidget(m_enemyCount);

    // --- 玩家2 ---
    auto* p2Layout = new QVBoxLayout();
    m_player2Label = new QLabel(QStringLiteral("P2"), this);
    m_player2Label->setStyleSheet("color: #0064FF; font-weight: bold; font-size: 15px;");
    m_player2Lives = new QLabel(QStringLiteral("HP: ♥♥♥"), this);
    m_player2Score = new QLabel(QStringLiteral("Score: 0"), this);
    m_player2Skill = new QLabel(QStringLiteral(""), this);
    m_player2Skill->setStyleSheet("color: #aaa; font-size: 11px;");
    p2Layout->addWidget(m_player2Label);
    p2Layout->addWidget(m_player2Lives);
    p2Layout->addWidget(m_player2Score);
    p2Layout->addWidget(m_player2Skill);

    mainLayout->addLayout(p1Layout);
    mainLayout->addStretch();
    mainLayout->addLayout(statusLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(p2Layout);
}

void HUD::attachToEngine(GameEngine* engine) {
    m_engine = engine;
    m_levelInfo->setText(QString("Level %1").arg(engine->currentLevel()));
    refresh();
}

void HUD::refresh() {
    if (!m_engine) return;

    // 玩家1
    int lives1 = m_engine->playerLives(0);
    QString hearts1;
    for (int i = 0; i < lives1; ++i) hearts1 += QStringLiteral("♥");
    if (hearts1.isEmpty()) hearts1 = "DEAD";
    m_player1Lives->setText(QString("HP: %1").arg(hearts1));
    m_player1Score->setText(QString("Score: %1").arg(m_engine->playerScore(0)));

    // 玩家1 技能状态
    for (const auto& t : m_engine->tanks()) {
        if (t->isPlayer() && t->playerIndex() == 0 && t->skill()) {
            auto* skill = t->skill();
            if (skill->isReady()) {
                m_player1Skill->setText(QString("Skill: %1 [Ready]").arg(skill->name()));
                m_player1Skill->setStyleSheet("color: #0f0; font-size: 11px;");
            } else {
                float pct = skill->cooldownPercent() * 100;
                m_player1Skill->setText(QString("Skill: %1 [%2%]")
                    .arg(skill->name()).arg(static_cast<int>(pct)));
                m_player1Skill->setStyleSheet("color: #888; font-size: 11px;");
            }
        }
    }

    // 玩家2
    if (m_engine->isMultiPlayer()) {
        int lives2 = m_engine->playerLives(1);
        QString hearts2;
        for (int i = 0; i < lives2; ++i) hearts2 += QStringLiteral("♥");
        if (hearts2.isEmpty()) hearts2 = "DEAD";
        m_player2Lives->setText(QString("HP: %1").arg(hearts2));
        m_player2Score->setText(QString("Score: %1").arg(m_engine->playerScore(1)));
        m_player2Label->setVisible(true);
        m_player2Lives->setVisible(true);
        m_player2Score->setVisible(true);
        m_player2Skill->setVisible(true);
    } else {
        m_player2Label->setVisible(false);
        m_player2Lives->setVisible(false);
        m_player2Score->setVisible(false);
        m_player2Skill->setVisible(false);
    }

    // 敌人数量
    m_enemyCount->setText(QString("Enemies: %1/%2")
        .arg(m_engine->remainingEnemies())
        .arg(m_engine->totalEnemies()));

    // 游戏状态
    if (m_engine->isGameOver()) {
        int w = m_engine->winner();
        if (w == 1) m_gameStatus->setText("VICTORY!");
        else if (w == 2) m_gameStatus->setText("P2 Wins!");
        else m_gameStatus->setText("GAME OVER");
        m_gameStatus->setStyleSheet("color: #e94560; font-size: 18px; font-weight: bold;");
    } else {
        m_gameStatus->setText("FIGHT!");
        m_gameStatus->setStyleSheet("color: yellow; font-size: 18px; font-weight: bold;");
    }
}
