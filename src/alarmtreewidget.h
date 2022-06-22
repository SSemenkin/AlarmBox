#ifndef ALARMTREEWIDGET_H
#define ALARMTREEWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include "alarminterrogator.h"
#include "settings.h"
#include "exceptionspanel.h"
#include "rbslocation.h"

class QLabel;

class AlarmTreeWidgetItem : public QTreeWidgetItem
{
public:
    AlarmTreeWidgetItem(const QString &pinnedText, Alarm::Category category) :
        QTreeWidgetItem(QStringList(pinnedText))
        , m_pinnedText(pinnedText)
        , m_category(category)
    {}

    AlarmTreeWidgetItem(const QStringList &labels) :
        QTreeWidgetItem(labels) {}



    const QString& pinnedText() const {
        return m_pinnedText;
    }

    QString comment() const {
        return text(m_commentColumn);
    }

    void setCategory(Alarm::Category cat) {
        m_category = cat;
    }

    Alarm::Category category() const {
        return m_category;
    }

private:
    QString m_pinnedText;
    Alarm::Category m_category;
    static int m_commentColumn;
};

struct DisplayAlarm {
    DisplayAlarm() = default;
    DisplayAlarm(const Alarm& alarm, AlarmTreeWidgetItem *alarmItem) : m_alarm(alarm)
      , m_alarmItem(alarmItem)
    {

    }
    Alarm alarm() const {
        return m_alarm;
    }

    AlarmTreeWidgetItem *treeWidgetItem() const {
        return m_alarmItem;
    }

    bool operator==(const DisplayAlarm& other) const {
        return m_alarm == other.m_alarm;
    }

    Alarm m_alarm;
    AlarmTreeWidgetItem *m_alarmItem {nullptr};
};


class AlarmTreeWidget : public QTreeWidget
{
    Q_OBJECT

    struct Velocity {
        int x {5};
        int y {5};
    };

public:
    AlarmTreeWidget(QWidget *parent = nullptr);
    ~AlarmTreeWidget();
    void processAlarms(const QVector<Alarm> &alarms);
    void onCurrentControllerChanged(const QString &controllerHostname);
    void onDVDVideoStateChanged(bool state);
    void execAddExceptionDialog();
    QString makeReport() const;
    QVector<Alarm> currentAlarms() const;

signals:
    void refresh();
    void updated();
    void activateRBSRequested(const QString &name, const QString &controllerHostname);

    void alarmRaised(const Alarm& alarm);
    void alarmCleared(const Alarm& alarm);

    void moveToItem(const QString &object);
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
    void processChildOfClearedAlarm(DisplayAlarm &alarm);

    void checkForClearedAlarms(const QVector<Alarm> &alarms);
    void checkForRaisedAlarms(const QVector<Alarm> &alarms);

    void loadExistingAlarms();
    void saveExistingAlarms();

    void loadUserComments();
    void saveUserComments();

    void resizeColumnsToContents();

    void getObjectLocation();
    QString getObjectLocation(QTreeWidgetItem *item) const;
    bool isItemTopLevel(QTreeWidgetItem *item) const;

    void setupContextMenu();


    void activateRBS();
    bool isSelectionRight();

    // just for fun
    void moveDvdVideoLabel();
    void setupDvdVideoLabel();

    void copyRow();


private:
    QVector<DisplayAlarm> m_alarms;
    QMap<QString, QMap<QString, AlarmComment>> m_userComments;
    QScopedPointer<ExceptionsPanel> m_exceptionsPanel;
    bool m_isManuallyRefreshed {false};
    RbsLocation m_location;

    AlarmTreeWidgetItem *m_dragParent {nullptr};
    AlarmTreeWidgetItem *m_dragItem {nullptr};

    static QString m_mimeDataFormat;
    Velocity m_velocity;
    QLabel *m_dvdVideoLabel;
    QTimer *m_dvdVideoTimer;
    QTimer *m_afkTimer;
    bool m_isDVDVideoEnabled;

    // QWidget interface
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;

    // QAbstractItemView interface
protected:
    virtual void startDrag(Qt::DropActions supportedActions) override;
    virtual void dropEvent(QDropEvent *event) override;
};

#endif // ALARMTREEWIDGET_H
