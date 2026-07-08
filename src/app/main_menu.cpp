/**
 * @file main_menu.cpp
 * @brief 主菜单实现
 */

#include "main_menu.h"
#include <QApplication>
#include <QFont>
#include <QMessageBox>
#include <QInputDialog>

MainMenu::MainMenu(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setWindowTitle(QString::fromUtf8("装甲核心：休闲坦克大战"));
    setFixedSize(500, 520);
    setStyleSheet("background-color: #1a1a2e;");
}

void MainMenu::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(10);

    // 标题
    m_titleLabel = new QLabel(QString::fromUtf8("装甲核心\n休闲坦克大战"), this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: #e94560; font-size: 34px; font-weight: bold; padding: 20px;");
    m_titleLabel->setFont(QFont("Microsoft YaHei", 34, QFont::Bold));
    layout->addWidget(m_titleLabel);

    // 副标题
    auto* subtitle = new QLabel("Armor Core: Casual Tank Battle v2.0", this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #aaaacc; font-size: 13px; padding-bottom: 15px;");
    layout->addWidget(subtitle);

    // 按钮
    QString btnBase = "QPushButton { background-color: #16213e; font-size: 18px; "
                      "border: 2px solid #0f3460; border-radius: 8px; padding: 10px; }"
                      "QPushButton:hover { background-color: #0f3460; color: white; }";

    m_singleBtn = createMenuButton(QString::fromUtf8("🎮 单人闯关"), btnBase);
    m_multiBtn = createMenuButton(QString::fromUtf8("⚔️ 双人对战"), btnBase);
    m_settingsBtn = createMenuButton(QString::fromUtf8("⚙️ 设置"), btnBase);
    m_highScoresBtn = createMenuButton(QString::fromUtf8("🏆 高分榜"), btnBase);
    m_exitBtn = createMenuButton(QString::fromUtf8("❌ 退出"),
        btnBase + "QPushButton:hover { background-color: #e94560; color: white; }");

    layout->addWidget(m_singleBtn);
    layout->addWidget(m_multiBtn);
    layout->addSpacing(15);
    layout->addWidget(m_settingsBtn);
    layout->addWidget(m_highScoresBtn);
    layout->addWidget(m_exitBtn);

    connect(m_singleBtn, &QPushButton::clicked, this, &MainMenu::onSinglePlayerClicked);
    connect(m_multiBtn, &QPushButton::clicked, this, &MainMenu::onMultiPlayerClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainMenu::settingsRequested);
    connect(m_highScoresBtn, &QPushButton::clicked, this, &MainMenu::highScoresRequested);
    connect(m_exitBtn, &QPushButton::clicked, qApp, &QApplication::quit);

    setLayout(layout);
}

QPushButton* MainMenu::createMenuButton(const QString& text, const QString& style) {
    auto* btn = new QPushButton(text, this);
    btn->setFixedSize(280, 50);
    btn->setCursor(Qt::PointingHandCursor);
    if (!style.isEmpty()) btn->setStyleSheet(style);
    return btn;
}

void MainMenu::onSinglePlayerClicked() {
    bool ok;
    int level = QInputDialog::getInt(this, QString::fromUtf8("选择关卡"),
        QString::fromUtf8("请输入关卡编号 (1-5):"), 1, 1, 5, 1, &ok);
    if (ok) emit startSingleGame(level);
}

void MainMenu::onMultiPlayerClicked() {
    QMessageBox::information(this, QString::fromUtf8("双人对战"),
        QString::fromUtf8("玩家1: W/A/S/D 移动, 空格 射击, Q 技能\n"
                          "玩家2: ↑/←/↓/→ 移动, 回车 射击, Shift 技能\n"
                          "ESC: 暂停游戏"));
    emit startMultiGame();
}
