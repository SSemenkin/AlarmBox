#include "controllerlistwidget.h"

#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <network/telnet.h>

ControllerListWidget::ControllerListWidget(QWidget *parent) :
    QListWidget(parent),
    m_okPixmap(":/icons/ok.png"),
    m_noOkPixmap(":/icons/no_ok.png"),
    m_contextMenu(new QMenu),
    m_reconnectAction(new QAction(QIcon(QPixmap(":/icons/reconnect.png")), tr("&Reconnect"))),
    m_editAction(new QAction(QIcon(QPixmap(":/icons/edit.png")), tr("&Edit"))),
    m_removeAction(new QAction(QIcon(QPixmap(":/icons/remove.webp")), tr("&Remove"))),
    m_detailAction(new QAction(QIcon(QPixmap(":/icons/details.png")), tr("&Details"))),
    m_addAction(new QAction(QIcon(QPixmap(":/icons/add.png")), tr("Add")))
{

    m_contextMenu->addAction(m_addAction.data());
    m_contextMenu->addAction(m_reconnectAction.data());
    m_contextMenu->addAction(m_editAction.data());
    m_contextMenu->addAction(m_removeAction.data());
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_detailAction.data());

    connect(m_addAction.data(), &QAction::triggered, this, &ControllerListWidget::addRequested);
    connect(m_reconnectAction.data(), &QAction::triggered, this, [this] () {
        callMethod(&ControllerListWidget::reconnectRequested);
    });
    connect(m_editAction.data(), &QAction::triggered, this, &ControllerListWidget::editRequested);
    connect(m_removeAction.data(), &QAction::triggered, this, &ControllerListWidget::removeController);
    connect(m_detailAction.data(), &QAction::triggered, this, [this] (){
        callMethod(&ControllerListWidget::detailRequested);
    });

}

void ControllerListWidget::processFailedControllerAuthentication(Telnet *telnet)
{
    addController(telnet);
    int result = QMessageBox::question(this, tr("Failure Authentication"), QString("Failed Authentication on %1!\n"
                                                              "Do you want try again?").arg(telnet->hostname()));

    if (result == QMessageBox::StandardButton::Yes) {
        emit reconnectRequested(telnet);
    }
}

void ControllerListWidget::processSuccessfullControllerAuthentication(Telnet *telnet)
{
    addController(telnet);
}

void ControllerListWidget::removeController()
{
    bool r = callMethod(&ControllerListWidget::removeRequested);
    if(r) {
        delete takeItem(row(selectedItems().first()));
    }
}

void ControllerListWidget::contextMenuEvent(QContextMenuEvent *event)
{

    QListWidgetItem *d = itemAt(event->pos());

    m_editAction->setEnabled(d);
    m_removeAction->setEnabled(d);
    m_detailAction->setEnabled(d);
    m_reconnectAction->setEnabled(d);
    m_contextMenu->popup(mapToGlobal(event->pos()));

    QListWidget::contextMenuEvent(event);
}

void ControllerListWidget::addController(Telnet *telnet)
{
    if (!m_controllersHosts.contains(telnet->hostname())) {
        addItem(telnet->hostname());
        m_controllersHosts.push_back(telnet->hostname());
        m_hostToRow[telnet->hostname()] = count() - 1;
    }

    QListWidgetItem *d = item(m_hostToRow[telnet->hostname()]);
    if (telnet->isLoggedInNode()) {
        d->setIcon(QIcon(m_okPixmap));
    } else {
        d->setIcon(QIcon(m_noOkPixmap));
    }
}

bool ControllerListWidget::callMethod(void (ControllerListWidget::*method)(const QString &))
{
    QList<QListWidgetItem*> d_items = selectedItems();
    if (d_items.isEmpty() || d_items.size() > 1) {
            QMessageBox::information(this, tr("Select more than one item"), tr("Please select only one item"));
            return false;
    }
    emit (this->*method)(d_items.first()->text());
    return true;
}
