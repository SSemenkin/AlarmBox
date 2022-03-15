#ifndef ALARMDISPLAYWIDGET_H
#define ALARMDISPLAYWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "alarminterrogator.h"

namespace Ui {
class AlarmDisplayWidget;
}

class AlarmDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmDisplayWidget(QWidget *parent = nullptr);
    ~AlarmDisplayWidget();

    void processAlarms(const QVector<Alarm> &alarms);
private:
    void addNewAlarm(const Alarm &alarm);
    QTreeWidgetItem* createItem(const Alarm& alarm) const;
private:
    Ui::AlarmDisplayWidget *ui;
    QVector<Alarm> m_currentAlarms;
};

#endif // ALARMDISPLAYWIDGET_H
