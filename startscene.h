#ifndef STARTSCENE_H
#define STARTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include "constants.h"

class StartScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit StartScene(QObject* parent = nullptr);

signals:
    void singlePlayerSelected();
    void twoPlayerSelected();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void updateSelection();
    void confirmSelection();

    QGraphicsTextItem* m_titleText;
    QGraphicsTextItem* m_singleItem;
    QGraphicsTextItem* m_pvpItem;
    QGraphicsTextItem* m_helpText;
    QGraphicsTextItem* m_versionText;

    int m_selectedIndex;  // 0 = 单人, 1 = 双人
};

#endif // STARTSCENE_H
