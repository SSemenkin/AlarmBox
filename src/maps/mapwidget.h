#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "RbsObject.h"

#include "types/alarm.h"
#include "custommapgraphicsview.h"

class MapLegend;

class MapWidget : public CustomMapGraphicsView
{
    Q_OBJECT
public:
    MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    void onAlarmRaised(const Alarm& alarm);
    void onAlarmCleared(const Alarm& alarm);
    void moveToItem(const QString &object);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void initObjects();
private:
    void markItemLikeAlarm(const QString &object, bool state = true);
    void markItemLikeMBL(const QString &object, bool state = true);
    void markItemLikeHalted(const QString &cellid);
    void markItemLikeNotWorks(const QString &cellid);
private:
    MapLegend *m_legend;
    QMap<QString, RbsObject*> m_rbsList;
};

#endif // MAPWIDGET_H
