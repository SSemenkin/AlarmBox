#include "nodestateview.h"
#include "addnodedialog.h"
#include <QAction>
#include "processholder.h"

NodeStateView::NodeStateView(QWidget *parent) :
    QTableView {parent}
{
    setupContextMenu();
}

void NodeStateView::setProcessHolder(QSharedPointer<ProcessHolder> processHolder)
{
    m_processHolder = processHolder;
}

void NodeStateView::setupContextMenu()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    QAction *add = new QAction(tr("Add"), this);
    QAction *nodes = new QAction(tr("Manage"), this);

    connect(add, &QAction::triggered, this, &NodeStateView::processAddNodeDialog);

    addAction(add);
    addAction(nodes);
}

void NodeStateView::processAddNodeDialog()
{
    AddNodeDialog dialog;
    connect(&dialog, &AddNodeDialog::addNodeRequested, this, [this] (int type, int vendor,
            const QString &name, const QString &dest) {
        m_processHolder->addNode(static_cast<Node::NodeType>(type),
                                 static_cast<Node::NodeVendor>(vendor),
                                 name, dest);
    });
    dialog.exec();
}
