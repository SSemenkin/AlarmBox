#ifndef ALARMTREEWIDGET_H
#define ALARMTREEWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include "alarminterrogator.h"

struct DisplayAlarm {
    DisplayAlarm() = default;
    DisplayAlarm(const Alarm& alarm, QTreeWidgetItem *alarmItem) : m_alarm(alarm)
      , m_alarmItem(alarmItem)
    {

    }
    Alarm m_alarm;
    QTreeWidgetItem *m_alarmItem {nullptr};

    bool operator==(const DisplayAlarm& other) const {
        return m_alarm == other.m_alarm;
    }
};

class AlarmTreeWidgetItem : public QTreeWidgetItem
{
public:
    AlarmTreeWidgetItem(const QString &pinnedText) :
        QTreeWidgetItem(QStringList(pinnedText))
        , m_pinnedText(pinnedText)
    {

    }
    const QString& pinnedText() const {
        return m_pinnedText;
    }


private:
    QString m_pinnedText;
};

class AlarmTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    AlarmTreeWidget(QWidget *parent = nullptr);
    void processAlarms(const QVector<Alarm> &alarms);
signals:
    void refresh();
private:
    void markItemLikeRaised(DisplayAlarm &alarm, const QBrush& color = Qt::red);
    void markItemLikeCleared(DisplayAlarm &alarm, const QBrush& color = Qt::green);
    void markItemLikeNormal(DisplayAlarm &alarm, const QBrush& color = Qt::NoBrush);
    void markTreeItemByBrush(QTreeWidgetItem *item, const QBrush& color);

    QTreeWidgetItem* createAlarmItem(const Alarm &alarm);
    void processNewAlarm(const Alarm &alarm);
private:
    QVector<DisplayAlarm> m_currentAlarms;
};

#endif // ALARMTREEWIDGET_H
