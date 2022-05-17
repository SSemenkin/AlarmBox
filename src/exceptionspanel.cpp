#include "exceptionspanel.h"
#include "settings.h"
#include "addexceptiondialog.h"
#include "controllerownership.h"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAction>
#include <QMessageBox>

ExceptionsPanel::ExceptionsPanel(QWidget *parent) :
    QTableWidget(parent)
  , m_exceptions(Settings::instance()->getExceptions())
{
    setWindowTitle(tr("Exceptions"));
    setWindowIcon(QIcon(":/icons/cs-themes.svg"));
    setColumnCount(3);
    setHorizontalHeaderLabels({tr("Controller"), tr("Object"), tr("Alarm")});
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    std::size_t rowCounter = 0;
    for (auto it = m_exceptions.begin(); it != m_exceptions.end(); ++it) {
        rowCounter += it.value().size();
    }
    setRowCount(rowCounter);

    rowCounter = 0;
    for (auto it = m_exceptions.begin(); it != m_exceptions.end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            setItem(rowCounter, 0, new QTableWidgetItem((*jt).m_controller));
            setItem(rowCounter, 1, new QTableWidgetItem((*jt).m_object));
            setItem(rowCounter++, 2, new QTableWidgetItem(titleToAlarmTypeIndex((*jt).m_alarmType.toInt())));
        }
    }
    setupContextMenu();
}

ExceptionsPanel::~ExceptionsPanel()
{
    Settings::instance()->setDisplayExceptions(m_exceptions);
}

void ExceptionsPanel::execAddExceptionDialog()
{
    AddExceptionDialog dialog(ControllerOwnership::instance()->controllerList());
    connect(&dialog, &AddExceptionDialog::exceptionAdded, this, &ExceptionsPanel::addException);
    dialog.exec();
}

bool ExceptionsPanel::isInException(const QString &controller, const QString &object, const QString &alarmType) const
{
    auto pos = m_exceptions.find(controller);
    if (pos == m_exceptions.end()) {
        return false;
    }
    DisplayException d(object, alarmType, controller);
    return (*pos).contains(d);
}

bool ExceptionsPanel::isInException(const Alarm &alarm) const
{
    return isInException(alarm.m_controllerTitle, alarm.m_object, QString::number(static_cast<int>(alarm.m_category)));
}

void ExceptionsPanel::addException(const QString &controller, const QString &object, const QString &alarmType)
{
    DisplayException exception(object, alarmType, controller);
    if (isDuplicate(exception)) {
        QMessageBox::warning(this, tr("Duplicate"), tr("This is duplicate of existing exception. Cannot add duplicate."));
        return;
    }

    m_exceptions[controller].push_back(exception);

    setRowCount(rowCount() + 1);
    setItem(rowCount() - 1, 0, new QTableWidgetItem(controller));
    setItem(rowCount() - 1, 1, new QTableWidgetItem(object));
    setItem(rowCount() - 1, 2, new QTableWidgetItem(titleToAlarmTypeIndex(alarmType.toInt())));
}

void ExceptionsPanel::removeException()
{
    if (rowCount() == 0) {
        return;
    }

    QList<QTableWidgetItem*> selectItems = selectedItems();

    if (selectItems.empty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select row."));
        return;
    }
    int row = selectItems.first()->row();
    DisplayException d(item(row, 1)->text(), indexToAlarmTitle(item(row, 2)->text()), item(row, 0)->text());

    bool r = m_exceptions[d.m_controller].removeOne(d);

    qDebug() << (r ? "Exception removed." : "Exception dont removed.");

    removeRow(row);
}

bool ExceptionsPanel::isDuplicate(const DisplayException &exception) const
{
    auto pos = m_exceptions.find(exception.m_controller);
    if (pos != m_exceptions.end()) {
        return (*pos).contains(exception);
    }
    return false;
}

void ExceptionsPanel::setupContextMenu()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    QAction *addException = new QAction(QIcon(":/icons/zanshin.svg"), tr("Add exception"), this);
    QAction *removeException = new QAction(QIcon(":/icons/com.azuredatastudio.oss.svg"), tr("Remove exception"), this);

    connect(addException, &QAction::triggered, this, &ExceptionsPanel::execAddExceptionDialog);
    connect(removeException, &QAction::triggered, this, &ExceptionsPanel::removeException);

    addAction(addException);
    addAction(removeException);
}


// static
QString ExceptionsPanel::titleToAlarmTypeIndex(int index)
{
    Q_ASSERT_X(index >= 0 && index < alarmTitles().size(), Q_FUNC_INFO, "Index out of range.");
    return alarmTitles().at(index);
}

// static
int ExceptionsPanel::indexToAlarmTitle(const QString &title)
{
    return alarmTitles().indexOf(title);
}

// static
QStringList ExceptionsPanel::alarmTitles()
{
    static QStringList d {tr("CF Alarm"), tr("Manually blocked"),
                          tr("Halted"),   tr("Not works")};
    return d;
}
