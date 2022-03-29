#ifndef ALARMTREEWIDGET_H
#define ALARMTREEWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include "alarminterrogator.h"
#include "settings.h"
#include "exceptionspanel.h"
#include "rbslocation.h"

class AlarmTreeWidgetItem : public QTreeWidgetItem
{
public:
    AlarmTreeWidgetItem(const QString &pinnedText) :
        QTreeWidgetItem(QStringList(pinnedText))
        , m_pinnedText(pinnedText)
    {

    }

    AlarmTreeWidgetItem(const QStringList &labels) :
        QTreeWidgetItem(labels) {}

    const QString& pinnedText() const {
        return m_pinnedText;
    }

    QString comment() const {
        return text(m_commentColumn);
    }

private:
    QString m_pinnedText;

    static int m_commentColumn;
};

struct DisplayAlarm {
    DisplayAlarm() = default;
    DisplayAlarm(const Alarm& alarm, AlarmTreeWidgetItem *alarmItem) : m_alarm(alarm)
      , m_alarmItem(alarmItem)
    {

    }
    Alarm m_alarm;
    AlarmTreeWidgetItem *m_alarmItem {nullptr};

    bool operator==(const DisplayAlarm& other) const {
        return m_alarm == other.m_alarm;
    }
};



class AlarmTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    AlarmTreeWidget(QWidget *parent = nullptr);
    ~AlarmTreeWidget();
    void processAlarms(const QVector<Alarm> &alarms);
    void onCurrentControllerChanged(const QString &controllerHostname);
    void execAddExceptionDialog();
signals:
    void refresh();
    void updated();
protected:
    virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;
private:
    void markItemLikeRaised(DisplayAlarm &alarm, const QBrush& color = Qt::red);
    void markItemLikeCleared(DisplayAlarm &alarm, const QBrush& color = Qt::green);
    void markItemLikeNormal(DisplayAlarm &alarm, const QBrush& color = Qt::NoBrush);
    void markTreeItemByBrush(QTreeWidgetItem *item, const QBrush& color);

    AlarmTreeWidgetItem* createAlarmItem(const Alarm &alarm);
    void processNewAlarm(const Alarm &alarm);
    void processClearedAlarm(DisplayAlarm& alarm);

    void checkForClearedAlarms(const QVector<Alarm> &alarms);
    void checkForRaisedAlarms(const QVector<Alarm> &alarms);

    void loadExistingAlarms();
    void saveExistingAlarms();

    void loadUserComments();
    void saveUserComments();

    void resizeColumnsToContents();

    void getObjectLocation();
    bool isItemTopLevel(QTreeWidgetItem *item) const;

    void setupContextMenu();

    QVector<Alarm> currentAlarms() const;
private:
    QVector<DisplayAlarm> m_alarms;
    QMap<QString, QMap<QString, AlarmComment>> m_userComments;
    QScopedPointer<ExceptionsPanel> m_exceptionsPanel;
    bool m_isManuallyRefreshed {false};
    RbsLocation m_location;
};

#endif // ALARMTREEWIDGET_H
