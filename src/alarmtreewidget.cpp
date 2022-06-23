#include "alarmtreewidget.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QDropEvent>
#include <QDataStream>
#include <QMimeData>
#include <QDrag>
#include <QLabel>
#include <QShortcut>
#include <QApplication>
#include <QClipboard>

namespace helpers{
    bool isDeltaBiggerThan(const QDateTime &begin, const QDateTime &end,
                           qint64 delta = 60 * 60 * 24 * 14)
    {
        QDateTime result;

        qint64 beginSecs = begin.toSecsSinceEpoch();
        qint64 endSecs = end.toSecsSinceEpoch();

        qint64 diff = endSecs > beginSecs ? endSecs - beginSecs :
                                            beginSecs - endSecs ;

        return diff > delta;
    }
}

int AlarmTreeWidgetItem::m_commentColumn = 3;
QString AlarmTreeWidget::m_mimeDataFormat = "application/alarms";

AlarmTreeWidget::AlarmTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
  , m_exceptionsPanel(new ExceptionsPanel)
  , m_location(Settings::instance()->getLocationFilepath())
  , m_dvdVideoLabel(new QLabel(this))
  , m_dvdVideoTimer(new QTimer(this))
  , m_afkTimer(new QTimer(this))
  , m_isDVDVideoEnabled(Settings::instance()->getIsDVDVideoEnabled())
{
    addTopLevelItem(new AlarmTreeWidgetItem(tr("CF Alarm"), Alarm::Category::A1));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Manually blocked"), Alarm::Category::A2));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Halted"), Alarm::Category::A3));
    addTopLevelItem(new AlarmTreeWidgetItem(tr("Not works"), Alarm::Category::A4));

    connect(this, &AlarmTreeWidget::itemClicked, this, [this] (QTreeWidgetItem *item, int column) {
        Q_UNUSED(column);
        if (AlarmTreeWidgetItem *aItem = dynamic_cast<AlarmTreeWidgetItem*>(item)) {
            if (aItem->category() == Alarm::Category::A1 ||
                aItem->category() == Alarm::Category::A2) {
                if (item->text(0).length() == 4)
                    emit moveToItem(item->text(0).left(3));
                else emit moveToItem(item->text(0));
            }
        }
    });


    for (int i = 0; i < topLevelItemCount(); ++i) {
        AlarmTreeWidgetItem *parentItem = static_cast<AlarmTreeWidgetItem*>(topLevelItem(i));
        parentItem->setText(0, parentItem->pinnedText() + "(0)");
    }

    expandAll();

    topLevelItem(0)->setIcon(0, QIcon(":/icons/folder-public.svg"));
    topLevelItem(1)->setIcon(0, QIcon(":/icons/folder-documents.svg"));
    topLevelItem(2)->setIcon(0, QIcon(":/icons/folder-templates.svg"));
    topLevelItem(3)->setIcon(0, QIcon(":/icons/folder.svg"));

    setupContextMenu();

    loadUserComments();
    loadExistingAlarms();

    setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    setWordWrap(true);
    setupDvdVideoLabel();

    connect(m_afkTimer, &QTimer::timeout, [this] () {
        if (!m_dvdVideoTimer->isActive()) {
            m_dvdVideoTimer->start();
            if (m_isDVDVideoEnabled) {
                m_dvdVideoLabel->show();
            }
        }
    });
    m_afkTimer->setInterval(60 * 1000 * 10);
    m_afkTimer->start();

    QShortcut *copyAction = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);

    connect(copyAction, &QShortcut::activated, this, &AlarmTreeWidget::copyRow);
}

AlarmTreeWidget::~AlarmTreeWidget()
{
    saveExistingAlarms();
    saveUserComments();
}

void AlarmTreeWidget::processAlarms(const QVector<Alarm> &alarms)
{
    checkForClearedAlarms(alarms);
    checkForRaisedAlarms(alarms);
    m_isManuallyRefreshed = false;
    resizeColumnsToContents();
    emit updated();
}

void AlarmTreeWidget::onCurrentControllerChanged(const QString &controllerHostname)
{
    for (int i = 0; i < m_alarms.size(); ++i) {
        if (m_alarms.at(i).alarm().m_state == Alarm::State::Normal) {
            markItemLikeNormal(m_alarms[i]);
        }
    }

    if (controllerHostname == tr("All")) {
        return;
    }

    for (int i = 0; i < m_alarms.size(); ++i) {
        DisplayAlarm &alarm = m_alarms[i];
        if ((alarm.m_alarm.m_state == Alarm::State::Normal &&
                controllerHostname == alarm.m_alarm.m_controllerTitle)
                ||
                (alarm.m_alarm.m_state == Alarm::State::Normal &&
                                controllerHostname == alarm.m_alarm.m_controller)) {
                    markItemLikeNormal(alarm, Qt::lightGray);
        }
    }
}

void AlarmTreeWidget::onDVDVideoStateChanged(bool state)
{
    m_isDVDVideoEnabled = state;
    if (m_dvdVideoTimer->isActive() && state) {
        m_dvdVideoLabel->show();
    } else if (!state && m_dvdVideoLabel->isVisible()) {
        m_dvdVideoLabel->hide();
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
    alarm.m_alarmItem->setText(5, alarm.m_alarm.m_clearedTime.toString(Qt::LocaleDate));
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

AlarmTreeWidgetItem *AlarmTreeWidget::createAlarmItem(const Alarm &alarm)
{
    QStringList labels;
    labels.push_back(alarm.m_object);
    labels.push_back(alarm.m_description);
    labels.push_back(alarm.m_controllerTitle);
    if (m_userComments.contains(alarm.m_controller)) {
        if (m_userComments[alarm.m_controller].contains(alarm.m_object)) {
            AlarmComment &comment = m_userComments[alarm.m_controller][alarm.m_object];
            //helpers::isDeltaBiggerThan(comment.m_createAt, alarm.m_raisedTime);
            labels.push_back(comment.m_description);
        } else labels.push_back("");
    }
    labels.push_back(alarm.m_raisedTime.toString(Qt::LocaleDate));
    labels.push_back(alarm.m_clearedTime.toString(Qt::LocaleDate));

    AlarmTreeWidgetItem *item = new AlarmTreeWidgetItem(labels);
    item->setCategory(alarm.m_category);
    item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
    return item;
}

void AlarmTreeWidget::processNewAlarm(const Alarm &alarm)
{
    qInfo() << "[Alarm Raised] " << alarm;

    AlarmTreeWidgetItem *parent;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        AlarmTreeWidgetItem *it = static_cast<AlarmTreeWidgetItem*>(topLevelItem(i));
        if (it->category() == alarm.m_category) {
            parent = it;
            break;
        }
    }
    // обработка исключений
    // не добавлять те аварии, которые находятся в исключении
    if (m_exceptionsPanel->isInException(alarm)) {
        return;
    }

    AlarmTreeWidgetItem *child = createAlarmItem(alarm);

    parent->addChild(child);
    parent->setText(0, parent->pinnedText() + "(" + QString::number(parent->childCount()) + ")");

    m_alarms.push_back(DisplayAlarm(alarm, child));
    markItemLikeRaised(m_alarms.last());
    emit alarmRaised(alarm);
}

void AlarmTreeWidget::processClearedAlarm(DisplayAlarm &alarm)
{
    qInfo() << "[Alarm Ceased] " << alarm.m_alarm;
    emit alarmCleared(alarm.m_alarm);

    processChildOfClearedAlarm(alarm);

    AlarmTreeWidgetItem *parent = static_cast<AlarmTreeWidgetItem*>(alarm.m_alarmItem->parent());
    parent->removeChild(alarm.m_alarmItem);
    m_alarms.removeOne(alarm);

    parent->setText(0, parent->pinnedText() + "(" + QString::number(parent->childCount()) + ")");
}

void AlarmTreeWidget::processChildOfClearedAlarm(DisplayAlarm &alarm)
{
    if (alarm.treeWidgetItem()->childCount() > 0) {
        QList<QTreeWidgetItem *> children;

        while(alarm.treeWidgetItem()->childCount()) {
            children.push_back(alarm.treeWidgetItem()->takeChild(0));
        }

        QTreeWidgetItem *topLevelParent = alarm.treeWidgetItem();
        while (topLevelParent->parent()) {
            topLevelParent = topLevelParent->parent();
        }

        topLevelParent->addChildren(children);
    }
}

void AlarmTreeWidget::checkForClearedAlarms(const QVector<Alarm> &alarms)
{
    for (int i = 0; i < m_alarms.size(); ++i) {
        DisplayAlarm &displayAlarm = m_alarms[i];
        Alarm &alarm = displayAlarm.m_alarm;

        if (!alarms.contains(alarm)) {
            if (alarm.isCleared()) {
                if (m_isManuallyRefreshed) {
                    processClearedAlarm(m_alarms[i--]);
                }
            } else if (m_exceptionsPanel->isInException(alarm)) {
                processClearedAlarm(displayAlarm);
            } else {
                markItemLikeCleared(displayAlarm);
            }
        }
    }
}

void AlarmTreeWidget::checkForRaisedAlarms(const QVector<Alarm> &alarms)
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
}

void AlarmTreeWidget::loadExistingAlarms()
{
    auto alarms = Settings::instance()->getExistingAlarms();
    std::sort(alarms.begin(), alarms.end());
    processAlarms(alarms);
}

void AlarmTreeWidget::saveExistingAlarms()
{
    Settings::instance()->setExistingAlarms(currentAlarms());
}

void AlarmTreeWidget::loadUserComments()
{
    m_userComments = Settings::instance()->getAlarmComments();
}

void AlarmTreeWidget::saveUserComments()
{
   for (int i = 0; i < m_alarms.size(); ++i) {
       const DisplayAlarm &d = m_alarms.at(i);
       if (!d.m_alarmItem->comment().isEmpty()) {
            AlarmComment a(d.m_alarm.m_object,
                           d.m_alarm.m_controller,
                           d.m_alarm.m_description,
                           d.m_alarmItem->comment());

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

void AlarmTreeWidget::resizeColumnsToContents()
{
    for (int i = 0; i < columnCount(); ++i) {
        resizeColumnToContents(i);
    }
}

void AlarmTreeWidget::getObjectLocation()
{
    if (!isSelectionRight()) {
        return;
    }
    QString object = selectedItems().first()->text(0);

    QMessageBox::information(this, object + tr(" location"), m_location.getLocation(object));
}

QString AlarmTreeWidget::getObjectLocation(QTreeWidgetItem *item) const
{
    QString object = item->text(0);
    return m_location.getLocation(object);
}

bool AlarmTreeWidget::isItemTopLevel(QTreeWidgetItem *item) const
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        if (item == topLevelItem(i)) {
            return true;
        }
    }
    return false;
}

void AlarmTreeWidget::setupContextMenu()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    QAction *refreshAction = new QAction(QIcon(":/icons/acetoneiso.svg"), tr("Refresh"), this);
    QAction *addExceptionAction = new QAction(QIcon(":/icons/cs-themes.svg"), tr("Exceptions"), this);
    QAction *getLocationAction = new QAction(QIcon(":/icons/gpxsee.svg"), tr("Location"), this);
    QAction *updateLocationsAction = new QAction(QIcon(":/icons/www-browser.svg"), tr("Update locations"), this);
    QAction *activateRBSAction = new QAction(QIcon(":/icons/battery.svg"), tr("Activate RBS"), this);

    connect(getLocationAction, &QAction::triggered, this, static_cast<void (AlarmTreeWidget::*)()>(&AlarmTreeWidget::getObjectLocation));
    connect(updateLocationsAction, &QAction::triggered, this, [this] () {
        UpdateStatus result = m_location.updateLocations(Settings::instance()->getLocationFilepath());
        if (result.m_status == Status::Duplicate) {
            QMessageBox::information(this, tr("Duplicates"), result.m_description);
        }
    });

    connect(activateRBSAction, &QAction::triggered, this, &AlarmTreeWidget::activateRBS);
    connect(addExceptionAction, &QAction::triggered, this, &AlarmTreeWidget::execAddExceptionDialog);
    connect(refreshAction, &QAction::triggered, this, [this](){
        m_isManuallyRefreshed = true;
        m_afkTimer->stop();
        m_afkTimer->start();
        m_dvdVideoTimer->stop();
        m_dvdVideoLabel->hide();
        emit refresh();
    });

    addAction(refreshAction);
    addAction(addExceptionAction);
    addAction(getLocationAction);
    addAction(updateLocationsAction);
    addAction(activateRBSAction);
}

QVector<Alarm> AlarmTreeWidget::currentAlarms() const
{
    QVector<Alarm> result;
    for (int i = 0; i < m_alarms.size(); ++i) {
        result.push_back(m_alarms.at(i).m_alarm);
    }
    return result;
}

void AlarmTreeWidget::activateRBS()
{
    if (!isSelectionRight()) {
        return;
    }

    QList<QTreeWidgetItem*> d = selectedItems();

    if (static_cast<AlarmTreeWidgetItem*>(d.first()->parent())->category() != Alarm::Category::A2) {
        return;
    } else {
        int choice = QMessageBox::question(this, tr("Activate RBS"), tr("Are you sure you want to unblock ") +
                              d.first()->text(0) + "?");

        if (choice != QMessageBox::Yes) {
            return;
        }
        for (int i = 0; i < m_alarms.size(); ++i) {
            const DisplayAlarm &alarm = m_alarms.at(i);
            if (alarm.m_alarmItem == d.first()) {
                if (alarm.m_alarm.isCleared()) {
                    QMessageBox::information(this, tr("Error"), tr("Alarm already cleared. Object is not manually blocked."));
                    return;
                }

                emit activateRBSRequested(alarm.m_alarm.m_object, alarm.m_alarm.m_controller);
                break;
            }
        }
    }
}

bool AlarmTreeWidget::isSelectionRight()
{
    QList<QTreeWidgetItem*> d = selectedItems();

    if (d.isEmpty() || d.size() > 1) {
        return false;
    }

    if (isItemTopLevel(d.first())) {
        return false;
    }

    return true;
}

void AlarmTreeWidget::moveDvdVideoLabel()
{
    if (m_dvdVideoLabel->geometry().x() + m_dvdVideoLabel->width() >= viewport()->width() ||
        m_dvdVideoLabel->geometry().x() + m_velocity.x < 0) {
        m_velocity.x *= -1;
    }

    if (m_dvdVideoLabel->geometry().y() + m_dvdVideoLabel->height() + m_velocity.y >= viewport()->height() ||
        m_dvdVideoLabel->geometry().y() + m_velocity.y < 0) {
        m_velocity.y *= -1;
    }
    m_dvdVideoLabel->move(m_dvdVideoLabel->x() + m_velocity.x, m_dvdVideoLabel->y() + m_velocity.y);
}

void AlarmTreeWidget::setupDvdVideoLabel()
{
    m_dvdVideoLabel->setPixmap(QPixmap(":/icons/dvd_video.png").scaled(300, 200, Qt::AspectRatioMode::KeepAspectRatio));
    m_dvdVideoTimer->setInterval(20);
    m_dvdVideoLabel->hide();
    connect(m_dvdVideoTimer, &QTimer::timeout, this, &AlarmTreeWidget::moveDvdVideoLabel);
}

void AlarmTreeWidget::copyRow()
{
    if (isSelectionRight()) {
        QList<QTreeWidgetItem*> items = selectedItems();

        QString clipboardText = items.first()->text(0) + " " + items.first()->text(1) + " " + items.first()->text(3);
        qApp->clipboard()->setText(clipboardText);
    }
}

QString AlarmTreeWidget::makeReport() const
{
    QString report = "2G:\n";

    for (int i = 0; i < topLevelItemCount(); ++i) {
        for (int j = 0; j < topLevelItem(i)->childCount(); ++j) {
            QTreeWidgetItem *alarmItem = topLevelItem(i)->child(j);
            report += alarmItem->text(0)  + "(" + getObjectLocation(alarmItem) + ") " + alarmItem->text(1) + " " + alarmItem->text(3) + '\n';
        }
        report += '\n';
    }

    return report;
}

void AlarmTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(m_mimeDataFormat)) {
        event->acceptProposedAction();
    } else QWidget::dragEnterEvent(event);
}

void AlarmTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(m_mimeDataFormat)) {
        event->acceptProposedAction();
    } else QWidget::dragMoveEvent(event);
}

void AlarmTreeWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);
    QList<QTreeWidgetItem*> d_items = selectedItems();
    if (d_items.isEmpty() || d_items.size() > 1 ) {
        return;
    }

    if (isItemTopLevel(d_items.first())) {
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;


    if (AlarmTreeWidgetItem* a_item = dynamic_cast<AlarmTreeWidgetItem*>(d_items.first())) {
        mimeData->setData(m_mimeDataFormat, a_item->comment().toUtf8());


        AlarmTreeWidgetItem *category_parent = nullptr;
        QTreeWidgetItem *tmp = a_item->parent();
        for (;;) {
            if (tmp->parent()) {
                tmp = tmp->parent();
            } else {
                break;
            }
        }
        category_parent = static_cast<AlarmTreeWidgetItem*>(tmp);
        m_dragParent = category_parent;
        m_dragItem = a_item;
    }


    QRect itemRect = visualItemRect(d_items.first());
    itemRect = itemRect.adjusted(0, itemRect.height() * 1.25 , 0, itemRect.height() * 1.25);

    drag->setMimeData(mimeData);
    drag->setPixmap(grab(itemRect));

    drag->exec();
}

void AlarmTreeWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();

    if (!data->hasFormat(m_mimeDataFormat)) {
        qDebug() << "drop accepted";
        QTreeWidget::dropEvent(event);
        return;
    }

    QTreeWidgetItem *dropOnItem = itemAt(event->pos());

    if(event->pos().x() < 60) {
        return;
    }

    if (!dropOnItem) {
        qDebug() << "Drop not accepted: not item;";
        return;
    }

    if (static_cast<AlarmTreeWidgetItem*>(dropOnItem)->category() != m_dragItem->category()) {
        qDebug() << "Drop not accepted: different category;";
        return;
    }

    if (isItemTopLevel(dropOnItem) &&
        m_dragItem->category() != (static_cast<AlarmTreeWidgetItem*>(dropOnItem))->category()) {
        qDebug() << "Drop not accepted: drop on top level && category drag different in top level;";
        return;
    }

    if (isItemTopLevel(dropOnItem) && m_dragItem->parent() == dropOnItem) {
        qDebug() << "Drop not accepted: drop on top level && dragItem == dropOnItem;";
        return;
    }


    QTreeWidget::dropEvent(event);
}

bool AlarmTreeWidget::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    if (index.parent().isValid())
        return QAbstractItemView::edit(index.model()->index(index.row(), 3, index.parent()), trigger, event);
    else return QAbstractItemView::edit(index, trigger, event);
}
