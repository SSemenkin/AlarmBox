#include "mapwidget.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "maps/qsqliteworker.h"
#include "maps/RbsObject.h"
#include "maps/custommapgraphicsview.h"
#include "maps/maplegend.h"

MapWidget::MapWidget(QWidget *parent) :
    CustomMapGraphicsView(new MapGraphicsScene(parent), parent)
  , m_legend(new MapLegend(tr("Legend"), this))
{
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    setTileSource(osmTiles);

    m_legend->addDefinition(Cell::alarmPixmap(), tr("In alarm"));
    m_legend->addDefinition(Cell::mblPixmap(), tr("MBL"));
    m_legend->addDefinition(Cell::normalPixmap(), tr("Normal"));

    initObjects();

    QObject::connect(this, &MapGraphicsView::zoomLevelChanged, this, [=] (quint8 zoomLevel) {
        bool required = zoomLevel > 11;

        for(auto& rbs : m_rbsList){
            rbs->showName(required);
        }
    });
    setZoomLevel(10);
    centerOn(QPointF(39.354178, 48.581492));
}

MapWidget::~MapWidget()
{
    for (auto it = m_rbsList.begin(); it != m_rbsList.end(); ++it) {
        delete it.value();
    }
}

void MapWidget::onAlarmRaised(const Alarm &alarm)
{
    switch(alarm.m_category) {
    case Alarm::Category::A1:
        markItemLikeAlarm(alarm.m_object);
        break;
    case Alarm::Category::A2:
        markItemLikeMBL(alarm.m_object);
        break;
    case Alarm::Category::A3:
        break;
    case Alarm::Category::A4:
        break;
    }
}

void MapWidget::onAlarmCleared(const Alarm &alarm)
{
    switch(alarm.m_category) {
    case Alarm::Category::A1:
        markItemLikeAlarm(alarm.m_object, false);
        break;
    case Alarm::Category::A2:
        markItemLikeMBL(alarm.m_object, false);
        break;
    case Alarm::Category::A3:
        break;
    case Alarm::Category::A4:
        break;
    }
}

void MapWidget::moveToItem(const QString &object)
{
    if (m_rbsList.contains(object)) {
        centerOn(m_rbsList[object]->pos());
        setZoomLevel(15);
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    CustomMapGraphicsView::resizeEvent(event);
    m_legend->move(width() - m_legend->width() - 20,
                   20);
    m_legend->update();
}

void MapWidget::initObjects()
{
    QSqliteWorker worker;

    QMap<QString , QPointF> rbsList = worker.getRbsList();

    int rbsCount = rbsList.size();
    QMap<QString , QVector<CellInfo>> cellList = worker.getCellList();


    RbsObject *object = nullptr;
    for (auto it = rbsList.begin(); it != rbsList.end(); ++it) {
        object = new RbsObject(it.key(), it.value());
        object->setToolTip(it.key());

        for (auto jt = m_rbsList.begin(); jt != m_rbsList.end(); ++jt) {
            if(jt.value()->pos() == it.value()) {
                object->setStacked(1);
                rbsCount--;
                break;
            }
        }
        m_rbsList.insert(object->getName(), object);



        scene()->addObject(object);
        object->setPos(it.value());

        auto rbsCells = cellList.find(it.key());

        if (rbsCells == cellList.end()) {
            qDebug() << "Cell list for " << it.key() << " not found.";
            qDebug() << cellList.keys();
        } else {
            for (int i = 0; i < rbsCells->size(); ++i) {
                object->addCell(rbsCells->at(i).cellname, rbsCells->at(i).angle);
            }
        }
    }

}

void MapWidget::markItemLikeAlarm(const QString &object, bool state)
{
    if (m_rbsList.contains(object)) {
        m_rbsList[object]->setDisabledState(state);
    } else if (m_rbsList.contains(object.left(object.length() - 1))) {
        m_rbsList[object.left(object.length() - 1)]->setDisabledState(state);
    }
}

void MapWidget::markItemLikeMBL(const QString &object, bool state)
{
    if (m_rbsList.contains(object)) {
        m_rbsList[object]->setBlockedState(state);
    } else if (m_rbsList.contains(object.left(object.length() - 1))) {
        m_rbsList[object.left(object.length() - 1)]->setBlockedState(state);
    }
}

void MapWidget::markItemLikeHalted(const QString &cellid)
{

}

void MapWidget::markItemLikeNotWorks(const QString &cellid)
{

}
