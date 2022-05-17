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

    for (int i = 0 ; i < objects.size(); ++i) {
        if (visibleRegion().intersects(objects.at(i)->boundingRect().toRect())) {
            if (RbsObject *rbs = dynamic_cast<RbsObject*>(objects.at(i))) {
                qDebug() << "Intersects with " << rbs->getName();
            } else {
                qDebug() << "Failed cast";
            }
        }
    }
}
