#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "constants.h"

class QGraphicsView;
class QGraphicsScene;
class GameScene;
class GameManager;
class StartScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onNewSinglePlayer();
    void onNewTwoPlayer();
    void onPause();
    void onQuit();
    void onAbout();
    void onGameOver(bool won);
    void showStartScreen();

private:
    void createMenus();
    void createUI();
    void updateStatusBar();

    QGraphicsView* m_view;
    GameScene* m_scene;
    StartScene* m_startScene;
    GameManager* m_manager;

    QLabel* m_statusLabel;
    QLabel* m_scoreLabel;
};

#endif // MAINWINDOW_H
