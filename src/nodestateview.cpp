#include "nodestateview.h"
#include "addnodedialog.h"
#include <QAction>

NodeStateView::NodeStateView(QWidget *parent) :
    QTableView {parent}
{
    setupContextMenu();
}

void NodeStateView::setupContextMenu()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    QAction *add = new QAction("Add", this);
    QAction *nodes = new QAction("Nodes", this);

    connect(add, &QAction::triggered, this,[]() {
        AddNodeDialog dialog;
        dialog.exec();
    });

    addAction(add);
    addAction(nodes);
}
