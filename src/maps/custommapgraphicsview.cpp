#include "custommapgraphicsview.h"
#include <QDebug>
#include <QPainter>
#include "RbsObject.h"

CustomMapGraphicsView::CustomMapGraphicsView(MapGraphicsScene *scene, QWidget *parent) :
    MapGraphicsView(scene, parent)
{
    connect(this, &MapGraphicsView::zoomLevelChanged, this, &CustomMapGraphicsView::onZoomLevelChanged);
}

void CustomMapGraphicsView::onZoomLevelChanged(uint8_t zoomLevel)
{
    QList<MapGraphicsObject*> objects = scene()->objects();
}
