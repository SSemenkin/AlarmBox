#include "alarmdisplaywidget.h"
#include "ui_alarmdisplaywidget.h"

AlarmDisplayWidget::AlarmDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmDisplayWidget)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setHeaderLabels({tr("Object"), tr("Alarm"), tr("Controller") ,
                                    tr("Raised time"), tr("Comment")});
    ui->treeWidget->addTopLevelItem(new TreeItem(tr("InAlarm")));
    ui->treeWidget->addTopLevelItem(new TreeItem(tr("Manually blocked")));
    ui->treeWidget->addTopLevelItem(new TreeItem(tr("Halted")));
    ui->treeWidget->addTopLevelItem(new TreeItem(tr("Not works")));
}

AlarmDisplayWidget::~AlarmDisplayWidget()
{
    delete ui;
}

void AlarmDisplayWidget::processAlarms(const QVector<Alarm> &alarms)
{
    QVector<DisplayAlarm> foundedAlarms;
    for (int i = 0; i < alarms.size(); ++i) {
        auto alarm = std::find_if(m_currentAlarms.begin(),
                               m_currentAlarms.end(), [&alarms, i](const DisplayAlarm& displayAlarm){
            return displayAlarm.m_alarm == alarms.at(i);
        });
        if (alarm == m_currentAlarms.end()) {
            addNewAlarm(alarms.at(i));
        } else {
            markAlarmNormal(*alarm);
            foundedAlarms.push_back(*alarm);
        }
    }

    for (int i = 0; i < m_currentAlarms.size(); ++i) {
        if (!foundedAlarms.contains(m_currentAlarms.at(i))) {
            markAlarmCleared(m_currentAlarms[i]);
        }
    }
}

void AlarmDisplayWidget::addNewAlarm(const Alarm &alarm)
{
    TreeItem *parent = static_cast<TreeItem*>(ui->treeWidget->topLevelItem(static_cast<int>(alarm.m_category)));

    QTreeWidgetItem *child = createItem(alarm);
    parent->addChild(child);
    parent->setText(0, parent->originalText() + "(" + QString::number(parent->childCount()) + ")");

    m_currentAlarms.push_back(DisplayAlarm(alarm, child));
}

QTreeWidgetItem *AlarmDisplayWidget::createItem(const Alarm &alarm) const
{
    QTreeWidgetItem *d = new QTreeWidgetItem(QStringList{alarm.m_object, alarm.m_description,
                                               alarm.m_controller, alarm.m_raisedTime.toString(Qt::LocaleDate),
                                               alarm.m_userComment});
    for (int i = 0; i < d->columnCount(); ++i) {
        d->setBackground(i, Qt::red);
    }
    return d;
}

void AlarmDisplayWidget::markAlarmCleared(DisplayAlarm &alarm)
{
    alarm.m_alarm.m_clearedTime = QDateTime::currentDateTime();
    alarm.m_alarm.m_state = Alarm::State::Cleared;

    for (int i = 0; i < alarm.m_alarmItem->columnCount(); ++i) {
        alarm.m_alarmItem->setBackground(i, Qt::green);
    }
}

void AlarmDisplayWidget::markAlarmNormal(DisplayAlarm &alarm)
{
    alarm.m_alarm.m_state = Alarm::State::Normal;

    for (int i = 0; i < alarm.m_alarmItem->columnCount(); ++i) {
        alarm.m_alarmItem->setBackground(i, Qt::NoBrush);
    }
}
