#ifndef ALARMDISPLAYWIDGET_H
#define ALARMDISPLAYWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "alarminterrogator.h"

namespace Ui {
class AlarmDisplayWidget;
}

struct DisplayAlarm {
    DisplayAlarm() = default;
    DisplayAlarm(const Alarm& alarm,
                 QTreeWidgetItem *alarmItem) : m_alarm(alarm)
      , m_alarmItem(alarmItem) {

    }
    Alarm m_alarm;
    QTreeWidgetItem *m_alarmItem {nullptr};

    bool operator==(const DisplayAlarm& other) const {
        return m_alarm == other.m_alarm;
    }
};

class TreeItem : public QTreeWidgetItem
{
public:
    TreeItem(const QString &text) :
        QTreeWidgetItem(QStringList(text))
      , m_original(text){

    }

    const QString &originalText() const {
        return m_original;
    }

    void setOriginal(const QString &text) {
        m_original = text;
    }

private:
    QString m_original;
};

class AlarmDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmDisplayWidget(QWidget *parent = nullptr);
    ~AlarmDisplayWidget();

    void processAlarms(const QVector<Alarm> &alarms);
signals:
    void refreshRequested();
private:
    void addNewAlarm(const Alarm &alarm);
    QTreeWidgetItem* createItem(const Alarm& alarm) const;
    void markAlarmNormal(DisplayAlarm& alarm);
    void markAlarmCleared(DisplayAlarm& alarm);
private:
    Ui::AlarmDisplayWidget *ui;
    QVector<DisplayAlarm> m_currentAlarms;
};

#endif // ALARMDISPLAYWIDGET_H
