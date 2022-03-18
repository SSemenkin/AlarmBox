#include "alarmtreewidget.h"

#include <QAction>

AlarmTreeWidget::AlarmTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
  , m_exceptionsPanel(new ExceptionsPanel)
{
    loadUserComments();
    addTopLevelItem(new AlarmTreeWidgetItem(tr("CF Alarm")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Manually blocked")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Halted")));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Not works")));

    topLevelItem(0)->setIcon(0, QIcon(":/icons/places/48/folder-public.svg"));
    topLevelItem(1)->setIcon(0, QIcon(":/icons/places/48/folder-documents.svg"));
    topLevelItem(2)->setIcon(0, QIcon(":/icons/places/48/folder-templates.svg"));
    topLevelItem(3)->setIcon(0, QIcon(":/icons/places/48/folder.svg"));

    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    QAction *refreshAction = new QAction(QIcon(":/icons/apps/scalable/catfish.svg"), tr("Refresh"), this);
    QAction *addExceptionAction = new QAction(QIcon(":/icons/preferences/scalable/cs-themes.svg"), tr("Exceptions"), this);
    connect(addExceptionAction, &QAction::triggered, this, &AlarmTreeWidget::execAddExceptionDialog);
    connect(refreshAction, &QAction::triggered, this, [this](){
        m_isManuallyRefreshed = true;
        emit refresh();
    });
    addAction(refreshAction);
    addAction(addExceptionAction);
}

AlarmTreeWidget::~AlarmTreeWidget()
{
    saveUserComments();
}

void AlarmTreeWidget::processAlarms(const QVector<Alarm> &alarms)
{
    for (int i = 0; i < alarms.size(); ++i) {
        auto alarm = std::find_if(m_alarms.begin(),
                               m_alarms.end(), [&alarms, i](const DisplayAlarm& displayAlarm){
            return displayAlarm.m_alarm == alarms.at(i) ;
        });

        if (alarm == m_alarms.end()) {
            processNewAlarm(alarms.at(i));
        } else {
            if(m_isManuallyRefreshed) {
                markItemLikeNormal(*alarm);
            }
        }
    }

    for (int i = 0; i < m_alarms.size(); ++i) {
        if (!alarms.contains(m_alarms.at(i).m_alarm)) {
            markItemLikeCleared(m_alarms[i]);
            if (m_isManuallyRefreshed) {
                processClearedAlarm(m_alarms[i--]);
            }
        }
    }
    m_isManuallyRefreshed = false;
}

void AlarmTreeWidget::onCurrentControllerChanged(const QString &controllerHostname)
{
    for (int i = 0; i < m_alarms.size(); ++i) {
        if (m_alarms.at(i).m_alarm.m_state == Alarm::State::Normal) {
            markItemLikeNormal(m_alarms[i]);
        }
    }

    if (controllerHostname == tr("All")) {
        return;
    }

    for (int i = 0; i < m_alarms.size(); ++i) {
        DisplayAlarm &alarm = m_alarms[i];
        if (alarm.m_alarm.m_state == Alarm::State::Normal &&
                controllerHostname == alarm.m_alarm.m_controller) {
                    markItemLikeNormal(alarm, Qt::lightGray);
        }
    }
}

void AlarmTreeWidget::execAddExceptionDialog()
{
    m_exceptionsPanel->resize(size());
    m_exceptionsPanel->show();
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
    labels.push_back(alarm.m_controllerTitle);
    labels.push_back(alarm.m_raisedTime.toString(Qt::LocaleDate));
    labels.push_back("");

    if (m_userComments.contains(alarm.m_controller)) {
        if (m_userComments[alarm.m_controller].contains(alarm.m_object)) {
            labels.push_back(m_userComments.value(alarm.m_controller).value(alarm.m_object).m_description);
        }
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(labels);
    item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
    return item;
}

void AlarmTreeWidget::processNewAlarm(const Alarm &alarm)
{
     AlarmTreeWidgetItem *parent = static_cast<AlarmTreeWidgetItem*>(topLevelItem(static_cast<int>(alarm.m_category)));
     // обработка исключений
     // не добавлять те аварии, которые находятся в исключении
     if (m_exceptionsPanel->isInException(alarm.m_controllerTitle, alarm.m_object,
                                          static_cast<AlarmTreeWidgetItem*>(parent)->pinnedText())) {
         return;
     }

     QTreeWidgetItem *child = createAlarmItem(alarm);
     parent->addChild(child);
     parent->setText(0, parent->pinnedText() + "(" + QString::number(parent->childCount()) + ")");

     m_alarms.push_back(DisplayAlarm(alarm, child));
     markItemLikeRaised(m_alarms.last());
}

void AlarmTreeWidget::processClearedAlarm(DisplayAlarm &alarm)
{
    AlarmTreeWidgetItem *parent = static_cast<AlarmTreeWidgetItem*>(alarm.m_alarmItem->parent());
    delete alarm.m_alarmItem;
    parent->setText(0, parent->pinnedText() + "(" + QString::number(parent->childCount()) + ")");
    m_alarms.removeOne(alarm);
}

void AlarmTreeWidget::loadUserComments()
{
    m_userComments = Settings::instance()->getAlarmComments();
}

void AlarmTreeWidget::saveUserComments()
{
   for (int i = 0; i < m_alarms.size(); ++i) {
       const DisplayAlarm &d = m_alarms.at(i);
       if (!d.m_alarmItem->text(5).isEmpty()) {
            AlarmComment a(d.m_alarm.m_object,
                           d.m_alarm.m_controller,
                           d.m_alarm.m_description,
                           d.m_alarmItem->text(5));

            if (m_userComments.contains(d.m_alarm.m_controller)) {
                auto controllerComments = m_userComments.value(d.m_alarm.m_controller);

                auto comment = controllerComments.find(d.m_alarm.m_object);

                if (comment != controllerComments.end()) {
                    if (comment.value() != a) {
                        m_userComments[d.m_alarm.m_controller][d.m_alarm.m_object] = a;
                    }
                } else {
                    m_userComments[d.m_alarm.m_controller][d.m_alarm.m_object] = a;
                }
            } else {
                m_userComments[d.m_alarm.m_controller][d.m_alarm.m_object] = a;
            }
       }
   }
   Settings::instance()->setAlarmComments(m_userComments);
}
