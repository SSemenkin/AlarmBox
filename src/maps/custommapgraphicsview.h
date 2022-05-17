#ifndef CUSTOMMAPGRAPHICSVIEW_H
#define CUSTOMMAPGRAPHICSVIEW_H

#include <MapGraphicsView.h>
#include <QObject>
#include <QWidget>

class CustomMapGraphicsView : public MapGraphicsView
{
    Q_OBJECT
public:
    CustomMapGraphicsView(MapGraphicsScene *scene = nullptr, QWidget *parent = nullptr);

protected:
    void onZoomLevelChanged(uint8_t zoomLevel);
};

#endif // CUSTOMMAPGRAPHICSVIEW_H
