#include "maps/maplegend.h"
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QDebug>

MapLegend::MapLegend(const QString &title, QWidget *parent) :
    QWidget(parent)
  , m_title(title)
{
}

void MapLegend::addDefinition(const QPixmap &pixmap, const QString &definition)
{
    m_definitions.push_back({pixmap, definition});
    setMinimumSize(150, m_definitions.size() * 65);
}

void MapLegend::paintEvent(QPaintEvent *event)
{
    (void) event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::lightGray);
    painter.drawRoundedRect(rect(), 5, 5);

    QFontMetrics metrics = fontMetrics();
    int verticalOffset = metrics.height() * 0.75;
    QRect titleRect = QRect(0, 0, width(), metrics.height() + verticalOffset);
    painter.drawText(titleRect, Qt::AlignCenter, m_title);

    int currentHeight = metrics.height() + verticalOffset * 2 + 10;
    for (auto it = m_definitions.begin(); it != m_definitions.end(); ++it) {
        QRect p_rect(5, currentHeight, 48, 48);
        painter.drawPixmap(p_rect, it->first);
        painter.drawText(QRectF(55, currentHeight + 10, width() - 35,metrics.height()),Qt::AlignVCenter, it->second);
        currentHeight += metrics.height() + verticalOffset + 10;
    }
}
