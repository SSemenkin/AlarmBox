#include "RbsObject.h"


RbsObject::RbsObject(const QString &name, const QPointF &pos, MapGraphicsObject *parent)
    : MapGraphicsObject(true, parent), __name(name), __position(pos), __matrix(nullptr),
      __cellCount(0), __umts(false), __currentSelection(false), __disabled(false)
{

    stackPosition = 0;
    QRectF box(0, 0, 110, 110);//80
    double width = box.width();
    double height = box.height();
    __rect = QRectF(-0.5 * width,
                    -0.5 * height,
                    width, height);

    //this->setFlag(MapGraphicsObjectFlag::ObjectIsSelectable, true);
    this->setFlag(MapGraphicsObjectFlag::ObjectIsFocusable, true);
    //this->setFlag(MapGraphicsObjectFlag::ObjectIsMovable, true);


    if(name.startsWith('3') || name.startsWith('4')){
        __umts = true;
    }
    __showName = false;


    connect(this, &RbsObject::leftClicked, this, [] (const QString &name) {
        qDebug() << Q_FUNC_INFO << name;
    });

    connect(this, &RbsObject::rightClicked, this, [] (const QString &name) {
        qDebug() << Q_FUNC_INFO << name;
    });

    connect(this, &RbsObject::doubleClicked, this, [] (const QString &name) {
        qDebug() << Q_FUNC_INFO << name;
    });
    connect(this, &RbsObject::callMenu, this, [] (const QString &name, const QPoint& point) {
        qDebug() << Q_FUNC_INFO << name << point;
    });


}

RbsObject::~RbsObject()
{
    for(Cell* cell : __cells){
        delete cell;
    }
    __cells.clear();
    if(__matrix)
        delete __matrix;
    //delete markPainter;
}

Cell* RbsObject::addCell(const QString &cellName, double azimut)
{
    Cell *cell = new Cell(cellName, &__position, azimut);
    cell->setPixmapSize(boundingRect().size().toSize());
    connect(cell, &Cell::stateChanged, this, &RbsObject::repaint);
    __cells.append(cell);
    cell->setCellNameVisible(__showName);
    QImage cellImage = cell->getCellPixmap().toImage();
    if(__matrix){
        __matrix->addCellImage(cellImage, __cellCount);
    } else {
        __matrix = new TrueMatrix(cellImage, __cellCount);
    }
    __cellCount++;
    return cell;
}

void RbsObject::showName(bool state)
{
    if(state != __showName){
        __showName = state;
        repaint();
    }
}

void RbsObject::setDisabledState(bool state)
{
    for(auto cell : __cells){
        if(cell->isDisabled() != state){
            cell->setDisabled(state);
        }
    }
    __disabled = state;
}

void RbsObject::setBlockedState(bool state)
{
    for(auto cell : __cells){
        cell->setBlocked(state);
    }
}

void RbsObject::setStacked(int position)
{
    stackPosition = position;
}

void RbsObject::setCurrentSelection(bool state)
{
    __currentSelection = state;
    repaint();
}

QString RbsObject::getName()
{
    return __name;
}

QPointF RbsObject::getPoint()
{
    return __position;
}

QList<Cell*> * RbsObject::getCells()
{
    return &__cells;
}

void RbsObject::setUmts(bool state = true)
{
    this->__umts = state;
}

QRectF RbsObject::boundingRect() const
{
    return __rect;
}

void RbsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QString cellName = clickedCell(event);
    if(cellName.size())
        emit doubleClicked(cellName);
}

void RbsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    double x = __rect.size().width()/2 + event->pos().x();
    double y = __rect.size().height()/2 - event->pos().y();
    int cellNumber = __matrix->shoot(x, y);
    if(cellNumber > -1){
        QString cellName = __cells[cellNumber]->getName();
        emit rightClicked(cellName);
    }
}

void RbsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton){
        double x = __rect.size().width()/2 + event->pos().x();
        double y = __rect.size().height()/2 - event->pos().y();
        int cellId = __matrix->shoot(x, y);
        if(cellId > -1){
            const QString &name = __cells[cellId]->getName();
            QPoint point = event->screenPos();
            emit callMenu(name, point);
        }
    }
    event->ignore();
}

void RbsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    bool marked = false;
    QFontMetrics metrics(qApp->font());
    for(Cell *cell : __cells){

        if(cell->isSelected()){
            marked = true;
        }
        painter->save();
        painter->drawImage(this->boundingRect(), cell->getCellPixmap().toImage());
        painter->restore();
    }

    if(__showName){
        painter->restore();
        painter->drawText(boundingRect().adjusted(0, -stackPosition * metrics.height() + 1, 0, -stackPosition * metrics.height() + 1), __name);
    }

    if(marked){
        painter->drawImage(-20, 45, QImage(":/mark3.png").mirrored(false, true));
        //QImage(__currentSelection ?  : ":/mark1.png").mirrored(false, true));
    }

}

void RbsObject::repaint()
{
    for(auto cell : __cells){
        cell->drawCell();
    }
    this->redrawRequested();
}


QString RbsObject::clickedCell(QGraphicsSceneMouseEvent *event)
{
    double x = __rect.size().width()/2 + event->pos().x();
    double y = __rect.size().height()/2 - event->pos().y();
    int cellNumber = __matrix->shoot(x, y);
    if(cellNumber > -1)
        return __cells[cellNumber]->getName();
    return QString();
}

void RbsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    MapGraphicsObject::mouseReleaseEvent(event);
}
