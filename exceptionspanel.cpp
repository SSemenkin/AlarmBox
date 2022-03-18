#include "exceptionspanel.h"
#include "settings.h"
#include "addexceptiondialog.h"
#include "controllerownership.h"

#include <QHeaderView>
#include <QTableWidgetItem>


ExceptionsPanel::ExceptionsPanel(QWidget *parent) :
    QTableWidget(parent)
  , m_exceptions(Settings::instance()->getExceptions())
{
    setColumnCount(3);
    setHorizontalHeaderLabels({tr("Controller"), tr("Object"), tr("Alarm")});

    std::size_t rowCount = 0;
    for (auto it = m_exceptions.begin(); it != m_exceptions.end(); ++it) {
        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            setItem(rowCount, 0, new QTableWidgetItem((*jt).m_controller));
            setItem(rowCount, 1, new QTableWidgetItem((*jt).m_object));
            setItem(rowCount++, 3, new QTableWidgetItem((*jt).m_alarmType));
        }
    }


}

ExceptionsPanel::~ExceptionsPanel()
{
    Settings::instance()->setDisplayExceptions(m_exceptions);
}

void ExceptionsPanel::execAddExceptionDialog()
{
    AddExceptionDialog dialog(ControllerOwnership::instance()->controllerList());
    dialog.exec();
}
