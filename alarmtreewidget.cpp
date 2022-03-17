#include "alarmtreewidget.h"

#include <QAction>

AlarmTreeWidget::AlarmTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    addTopLevelItem(new AlarmTreeWidgetItem(tr("In Alarm")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Manually blocked")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Halted")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Not works")));

    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    QAction *refreshAction = new QAction(QIcon(":/icons/refresh.png"), tr("Refresh"), this);
    connect(refreshAction, &QAction::triggered, this, [this](){
        isManuallyRefreshed = true;
        emit refresh();
    });
    addAction(refreshAction);
}

void AlarmTreeWidget::processAlarms(const QVector<Alarm> &alarms)
{
    for (int i = 0; i < alarms.size(); ++i) {
        auto alarm = std::find_if(m_currentAlarms.begin(),
                               m_currentAlarms.end(), [&alarms, i](const DisplayAlarm& displayAlarm){
            return displayAlarm.m_alarm == alarms.at(i);
        });
        if (alarm == m_currentAlarms.end()) {
            processNewAlarm(alarms.at(i));
        } else {
            if(isManuallyRefreshed) {
                markItemLikeNormal(*alarm);
            }
        }
    }

    for (int i = 0; i < m_currentAlarms.size(); ++i) {
        if (!alarms.contains(m_currentAlarms.at(i).m_alarm)) {
            markItemLikeCleared(m_currentAlarms[i]);
            if (isManuallyRefreshed) {
                processClearedAlarm(m_currentAlarms[i]);
            }
        }
    }
    isManuallyRefreshed = false;
}

void AlarmTreeWidget::onCurrentControllerChanged(const QString &controllerHostname)
{
    for (int i = 0; i < m_currentAlarms.size(); ++i) {
        if (m_currentAlarms.at(i).m_alarm.m_state == Alarm::State::Normal) {
            markItemLikeNormal(m_currentAlarms[i]);
        }
    }

    if (controllerHostname == tr("All")) {
        return;
    }

    for (int i = 0; i < m_currentAlarms.size(); ++i) {
        DisplayAlarm &alarm = m_currentAlarms[i];
        if (alarm.m_alarm.m_state == Alarm::State::Normal &&
                controllerHostname == alarm.m_alarm.m_controller) {
                    markItemLikeNormal(alarm, Qt::lightGray);
        }
    }
}

void AlarmTreeWidget::markItemLikeRaised(DisplayAlarm &alarm, const QBrush& brush)
{
    markTreeItemByBrush(alarm.m_alarmItem, brush);
}

void AlarmTreeWidget::markItemLikeCleared(DisplayAlarm &alarm, const QBrush& brush)
{
    alarm.m_alarm.m_clearedTime = QDateTime::currentDateTime();
    alarm.m_alarm.m_state = Alarm::State::Cleared;
    alarm.m_alarmItem->setText(4, alarm.m_alarm.m_clearedTime.toString(Qt::LocaleDate));
    markTreeItemByBrush(alarm.m_alarmItem, brush);
}

void AlarmTreeWidget::markItemLikeNormal(DisplayAlarm &alarm, const QBrush &brush)
{
    alarm.m_alarm.m_state = Alarm::State::Normal;
    markTreeItemByBrush(alarm.m_alarmItem, brush);
}

void AlarmTreeWidget::markTreeItemByBrush(QTreeWidgetItem *item, const QBrush &brush)
{
    for (int i = 0; i < item->columnCount(); ++i) {
        item->setBackground(i, brush);
    }
}

QTreeWidgetItem *AlarmTreeWidget::createAlarmItem(const Alarm &alarm)
{
    QStringList labels;
    labels.push_back(alarm.m_object);
    labels.push_back(alarm.m_description);
    labels.push_back(alarm.m_controller);
    labels.push_back(alarm.m_raisedTime.toString(Qt::LocaleDate));
    labels.push_back(alarm.m_userComment);

    QTreeWidgetItem *item = new QTreeWidgetItem(labels);
    item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
    return item;
}

void AlarmTreeWidget::processNewAlarm(const Alarm &alarm)
{
     AlarmTreeWidgetItem *parent = static_cast<AlarmTreeWidgetItem*>(topLevelItem(static_cast<int>(alarm.m_category)));
     QTreeWidgetItem *child = createAlarmItem(alarm);
     parent->addChild(child);
     parent->setText(0, parent->pinnedText() + "(" + QString::number(parent->childCount()) + ")");

     m_currentAlarms.push_back(DisplayAlarm(alarm, child));
     markItemLikeRaised(m_currentAlarms.last());
}

void AlarmTreeWidget::processClearedAlarm(DisplayAlarm &alarm)
{
    delete alarm.m_alarmItem;
    m_currentAlarms.removeOne(alarm);
}
