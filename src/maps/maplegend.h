#ifndef MAPLEGEND_H
#define MAPLEGEND_H

#include <QWidget>
#include <QMap>

class MapLegend : public QWidget
{
    Q_OBJECT
public:
    explicit MapLegend(const QString &title = tr("Legend"), QWidget *parent = nullptr);
    void addDefinition(const QPixmap &pixmap, const QString &definition);
signals:

protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QString m_title;
    QVector<QPair<QPixmap, QString>> m_definitions;
};

#endif // MAPLEGEND_H
