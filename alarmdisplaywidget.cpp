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
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(tr("InAlarm"))));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(tr("Manually blocked"))));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(tr("Halted"))));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList(tr("Not works"))));
}

AlarmDisplayWidget::~AlarmDisplayWidget()
{
    delete ui;
}

void AlarmDisplayWidget::processAlarms(const QVector<Alarm> &alarms)
{
    for (int i = 0; i < alarms.size(); ++i) {
        auto alarm = std::find(m_currentAlarms.begin(),
                               m_currentAlarms.end(),
                               alarms.at(i));
        if (alarm == m_currentAlarms.end()) {
            addNewAlarm(alarms.at(i));
        } else {

        }
    }
}

void AlarmDisplayWidget::addNewAlarm(const Alarm &alarm)
{
    m_currentAlarms.push_back(alarm);
    QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(static_cast<int>(alarm.m_category));
    parent->addChild(createItem(alarm));
}

QTreeWidgetItem *AlarmDisplayWidget::createItem(const Alarm &alarm) const
{
    return new QTreeWidgetItem(QStringList{alarm.m_object, alarm.m_description,
                                           alarm.m_controller, alarm.m_raisedTime.toString(Qt::LocaleDate),
                                           alarm.m_userComment});
}
