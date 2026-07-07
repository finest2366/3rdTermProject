#include "startscene.h"
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QPainter>
#include <QBrush>

StartScene::StartScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_selectedIndex(0)
{
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    setBackgroundBrush(GameColors::BACKGROUND);

    // === 标题 ===
    QFont titleFont("Microsoft YaHei", 36, QFont::Bold);
    m_titleText = addText(QString::fromUtf8("坦 克 大 战"), titleFont);
    m_titleText->setDefaultTextColor(GameColors::BULLET);  // 金色标题
    m_titleText->setPos(SCENE_WIDTH / 2.0 - m_titleText->boundingRect().width() / 2.0, 60);

    // 标题副行
    QFont subFont("Arial", 14);
    auto* subTitle = addText("TANK BATTLE", subFont);
    subTitle->setDefaultTextColor(GameColors::STEEL);
    subTitle->setPos(SCENE_WIDTH / 2.0 - subTitle->boundingRect().width() / 2.0, 115);

    // === 装饰性坦克图案（用 QGraphicsRectItem 拼出一个坦克） ===
    // 左侧玩家坦克
    QColor p1Color = GameColors::PLAYER1_BODY;
    QColor p1Dark = GameColors::PLAYER1_BARREL;
    qreal tankY = 260;
    qreal tank1X = SCENE_WIDTH / 2.0 - 120;

    // 车身
    auto* body1 = addRect(-16, -14, 32, 28, QPen(p1Dark, 2), QBrush(p1Color));
    body1->setPos(tank1X, tankY);
    body1->setZValue(1);
    // 炮管
    auto* barrel1 = addRect(-3, -24, 6, 22, QPen(Qt::NoPen), QBrush(p1Dark));
    barrel1->setPos(tank1X, tankY);
    barrel1->setZValue(1);
    // 履带
    auto* trackL1 = addRect(-16, -14, 5, 28, QPen(Qt::NoPen), QBrush(p1Dark));
    trackL1->setPos(tank1X, tankY);
    trackL1->setZValue(1);
    auto* trackR1 = addRect(11, -14, 5, 28, QPen(Qt::NoPen), QBrush(p1Dark));
    trackR1->setPos(tank1X, tankY);
    trackR1->setZValue(1);

    // 右侧敌人坦克
    QColor enemyColor = GameColors::ENEMY_BODY;
    QColor enemyDark = GameColors::ENEMY_BARREL;
    qreal tank2X = SCENE_WIDTH / 2.0 + 120;

    auto* body2 = addRect(-16, -14, 32, 28, QPen(enemyDark, 2), QBrush(enemyColor));
    body2->setPos(tank2X, tankY);
    body2->setZValue(1);
    auto* barrel2 = addRect(-3, 2, 6, 22, QPen(Qt::NoPen), QBrush(enemyDark));
    barrel2->setPos(tank2X, tankY);
    barrel2->setZValue(1);
    auto* trackL2 = addRect(-16, -14, 5, 28, QPen(Qt::NoPen), QBrush(enemyDark));
    trackL2->setPos(tank2X, tankY);
    trackL2->setZValue(1);
    auto* trackR2 = addRect(11, -14, 5, 28, QPen(Qt::NoPen), QBrush(enemyDark));
    trackR2->setPos(tank2X, tankY);
    trackR2->setZValue(1);

    // 中间的 "VS" 文字
    QFont vsFont("Arial", 20, QFont::Bold);
    auto* vsText = addText("VS", vsFont);
    vsText->setDefaultTextColor(Qt::white);
    vsText->setPos(SCENE_WIDTH / 2.0 - vsText->boundingRect().width() / 2.0, tankY - 12);

    // === 菜单项 ===
    QFont menuFont("Microsoft YaHei", 24);

    m_singleItem = addText(QString::fromUtf8("▶  单 人 闯 关"), menuFont);
    m_singleItem->setDefaultTextColor(GameColors::PLAYER1_BODY);
    m_singleItem->setPos(SCENE_WIDTH / 2.0 - m_singleItem->boundingRect().width() / 2.0, 350);

    m_pvpItem = addText(QString::fromUtf8("    双 人 对 战"), menuFont);
    m_pvpItem->setDefaultTextColor(GameColors::STEEL);
    m_pvpItem->setPos(SCENE_WIDTH / 2.0 - m_pvpItem->boundingRect().width() / 2.0, 400);

    // === 操作说明 ===
    QFont helpFont("Microsoft YaHei", 11);
    m_helpText = addText(
        QString::fromUtf8("↑↓ 选择    Enter 确认    或点击菜单项")
        , helpFont);
    m_helpText->setDefaultTextColor(GameColors::GRASS);
    m_helpText->setPos(SCENE_WIDTH / 2.0 - m_helpText->boundingRect().width() / 2.0, 480);

    // === 底部版本信息 ===
    QFont verFont("Arial", 9);
    m_versionText = addText("v1.0  |  Qt 6 + C++17  |  Classic Tank Battle", verFont);
    m_versionText->setDefaultTextColor(GameColors::STEEL.darker(150));
    m_versionText->setPos(SCENE_WIDTH / 2.0 - m_versionText->boundingRect().width() / 2.0, SCENE_HEIGHT - 30);
}

void StartScene::updateSelection()
{
    QFont menuFont("Microsoft YaHei", 24);

    if (m_selectedIndex == 0) {
        m_singleItem->setPlainText(QString::fromUtf8("▶  单 人 闯 关"));
        m_singleItem->setDefaultTextColor(GameColors::BULLET);  // 高亮金色
        m_pvpItem->setPlainText(QString::fromUtf8("    双 人 对 战"));
        m_pvpItem->setDefaultTextColor(GameColors::STEEL);
    } else {
        m_singleItem->setPlainText(QString::fromUtf8("    单 人 闯 关"));
        m_singleItem->setDefaultTextColor(GameColors::STEEL);
        m_pvpItem->setPlainText(QString::fromUtf8("▶  双 人 对 战"));
        m_pvpItem->setDefaultTextColor(GameColors::BULLET);  // 高亮金色
    }
}

void StartScene::confirmSelection()
{
    if (m_selectedIndex == 0) {
        emit singlePlayerSelected();
    } else {
        emit twoPlayerSelected();
    }
}

void StartScene::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        m_selectedIndex = (m_selectedIndex == 0) ? 1 : 0;
        updateSelection();
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        m_selectedIndex = (m_selectedIndex == 0) ? 1 : 0;
        updateSelection();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        confirmSelection();
        break;
    default:
        QGraphicsScene::keyPressEvent(event);
        break;
    }
}

void StartScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF pos = event->scenePos();

    // 检查是否点击了菜单项
    QRectF singleRect(
        m_singleItem->pos().x(),
        m_singleItem->pos().y(),
        m_singleItem->boundingRect().width(),
        m_singleItem->boundingRect().height()
    );
    QRectF pvpRect(
        m_pvpItem->pos().x(),
        m_pvpItem->pos().y(),
        m_pvpItem->boundingRect().width(),
        m_pvpItem->boundingRect().height()
    );

    if (singleRect.contains(pos)) {
        m_selectedIndex = 0;
        updateSelection();
        confirmSelection();
    } else if (pvpRect.contains(pos)) {
        m_selectedIndex = 1;
        updateSelection();
        confirmSelection();
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}
