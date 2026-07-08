/**
 * @file main_menu.h
 * @brief 主菜单界面 —— 单人/双人/设置/高分榜/退出
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class MainMenu : public QWidget {
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent = nullptr);
    ~MainMenu() override = default;

signals:
    void startSingleGame(int levelId);
    void startMultiGame();
    void settingsRequested();
    void highScoresRequested();

private slots:
    void onSinglePlayerClicked();
    void onMultiPlayerClicked();

private:
    void setupUI();
    QPushButton* createMenuButton(const QString& text, const QString& style = "");

    QLabel* m_titleLabel = nullptr;
    QPushButton* m_singleBtn = nullptr;
    QPushButton* m_multiBtn = nullptr;
    QPushButton* m_settingsBtn = nullptr;
    QPushButton* m_highScoresBtn = nullptr;
    QPushButton* m_exitBtn = nullptr;
};
