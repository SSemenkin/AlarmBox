#ifndef RBSOBJECT_H
#define RBSOBJECT_H

#include "MapGraphicsObject.h"
#include "cell.h"
#include "TrueMatrix.h"
#include <QMenu>
#include <QScopedPointer>

class RbsObject : public MapGraphicsObject
{
    Q_OBJECT

public:
    explicit RbsObject(const QString &name, const QPointF &pos,
              MapGraphicsObject *parent = 0);
    ~RbsObject();
    Cell *addCell(const QString &cellName, double azimut);
    void showName(bool state);
    void setDisabledState(bool state);
    void setBlockedState(bool state);
    void setStacked(int position);
    void setCurrentSelection(bool state = true);
    QString getName();
    QPointF getPoint();
    QList<Cell *> *getCells();
    void setUmts(bool state);

signals:
    void leftClicked(const QString &name);
    void rightClicked(const QString &name);
    void doubleClicked(const QString &name);
    void callMenu(const QString &name, const QPoint& point);

public slots:
    void repaint();
private:
    void processContextMenu(const QString &cellName,
                            const QPoint& point);
    QMenu* createContextMenu();

private:
    QString __name;
    QList<Cell*> __cells;
    QPointF __position;
    QRectF __rect;
    QString clickedCell(QGraphicsSceneMouseEvent *event);
    TrueMatrix *__matrix;
    QPainter *markPainter;
    int __cellCount;
    bool __umts;
    bool __showName;
    int stackPosition;
    bool __currentSelection;
    bool __disabled;
    bool __blocked;

    QScopedPointer<QMenu> m_contextMenu {nullptr};

    // MapGraphicsObject interface
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget=0) override;
    virtual QRectF boundingRect() const override;
};

#endif // RBSOBJECT_H
