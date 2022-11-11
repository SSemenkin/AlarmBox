#include "cell.h"
#include <QDebug>

Cell::Cell(QString name, QPointF *parrentPoint, double azimut)
    : __name(name), __azimut(azimut), __cellPixmap(nullptr),
      __size(QSize(90,90)), __currentColor(QColor(180,250,180,100)),
      __selected(false), __disabled(false), __blocked(false),
      __point(parrentPoint)
{
    drawCell();
}

Cell::~Cell()
{
    delete __cellPixmap;
}

QPixmap Cell::getCellPixmap()
{
    return *__cellPixmap;
}

void Cell::setAzimut(double azimut)
{
    __azimut = azimut;
    drawCell();
}

void Cell::setPixmapSize(QSize size)
{
    __size = size;
    drawCell();
}

void Cell::setPixmapSize(qreal width, qreal height)
{
    setPixmapSize(QSize(width, height));
}

const QString &Cell::getName()
{
    return __name;
}

QRectF Cell::getRect()
{
    return __cellPixmap->rect();
}

QPointF Cell::getPoint()
{
    return *__point;
}

void Cell::setValue(int value)
{
    setValueWithoutRepaint(value);
    emit stateChanged();
}

void Cell::setSelected(bool state)
{
    __selected = state;
    emit stateChanged();
}

void Cell::setDisabled(bool state)
{
    __disabled = state;
    if(state == false){
        __blocked = false;
    }
    emit stateChanged();
}

void Cell::setBlocked(bool state)
{
    __blocked = state;
    //__disabled = state;
    emit stateChanged();
}

void Cell::setValueWithoutRepaint(int value)
{
    QColor color(QColor::Spec::Hsv);
    if(value > 0){
    int colorValue = 110 - value;
    color.setHsv(colorValue, 255, 255, 100 + value * 0.7);
    } else {
        color.setHsv(280, 55, 55, 80);
    }
    __currentColor = color;
    drawCell();
}

bool Cell::isSelected()
{
    return __selected;
}

bool Cell::isDisabled()
{
    return __disabled;
}

bool Cell::isBlocked()
{
    return __blocked;
}

QPixmap Cell::mblPixmap()
{
    static QPixmap p(":/icons/map/mbl.png");
    return p;
}

QPixmap Cell::alarmPixmap()
{
    static QPixmap p(":/icons/map/alarm.png");
    return p;
}

QPixmap Cell::normalPixmap()
{
    static QPixmap p(":/icons/map/normal.png");
    return p;
}

void Cell::drawCell()
{
    if(__cellPixmap){
        delete __cellPixmap;
    }
    __cellPixmap = new QPixmap(__size);
    __cellPixmap->fill(Qt::transparent);
    QPainter painter(__cellPixmap);
    painter.setBackgroundMode(Qt::BGMode::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(__currentColor);

    QPen pen(QColor(30,30,30));

    double x = __size.width();
    double y = __size.height();
    double midX = x/2;
    double midY = x/2;

    painter.translate(x/2, y/2);
    qreal angle = 180 - __azimut;
    painter.rotate(angle);
    painter.translate(-x/2, -y/2);

    QPainterPath rectPath;

    if(__selected){
        pen.setWidth(3);
        painter.setPen(pen);

        QPainterPath selectionPath;
        selectionPath.moveTo(midX, midY);
        selectionPath.lineTo(midX * 0.7, y * 0.2);
        selectionPath.cubicTo(midX * 0.7, y * 0.2, midX, y * 0.1, midX * 1.3, y * 0.2);
        selectionPath.closeSubpath();
        painter.drawPath(selectionPath);
    } else {
        rectPath.moveTo(midX, midY);
        rectPath.lineTo(midX * 0.8, y * 0.3);
        rectPath.cubicTo(midX * 0.8, y * 0.3, midX, y * 0.2, midX * 1.2, y * 0.3);
        rectPath.closeSubpath();
    }

    if(__disabled){
        pen.setColor(Qt::red);
        painter.setBrush(Qt::black);//QColor(80,80,80));
        pen.setWidth(3);
    }

    if(__blocked){
        //pen.setColor(Qt::blue);
        painter.setBrush(Qt::blue);
        pen.setWidth(2);
    }

    painter.setPen(pen);
    if (isParentNameVisible)
        rectPath.addText(rectPath.pointAtPercent(0) + QPointF(0, rectPath.pointAtPercent(1).y()), qApp->font(), __name.right(1));
    painter.drawPath(rectPath);
}
