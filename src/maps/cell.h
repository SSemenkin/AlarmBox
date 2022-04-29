#ifndef CELL_H
#define CELL_H
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QColor>
#include <QApplication>

class Cell : public QObject
{
    Q_OBJECT
public:
    Cell(QString name, QPointF *parrentPoint, double azimut = 0);
    ~Cell();
    QPixmap getCellPixmap();
    void setAzimut(double azimut);
    void setPixmapSize(QSize size);
    void setPixmapSize(qreal width, qreal height);
    const QString &getName();
    QRectF getRect();
    QPointF getPoint();
    void drawCell();

    void setValue(int value);
    void setSelected(bool state = true);
    void setDisabled(bool state = true);
    void setBlocked(bool state = true);

    void setValueWithoutRepaint(int value);

    bool isSelected();
    bool isDisabled();
    bool isBlocked();
    void setCellNameVisible (bool state) {
        isParentNameVisible = state;
    }

signals:
    void stateChanged();

private:

    QString __name;
    double __azimut;
    QPixmap *__cellPixmap;
    QSize __size;
    QColor __currentColor;
    bool __selected;
    bool __disabled;
    bool __blocked;
    QPointF *__point;
    bool isParentNameVisible {false};
};


#endif // CELL_H
