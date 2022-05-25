#include "nodestateview.h"
#include "addnodedialog.h"
#include <QAction>
#include "processholder.h"
#include "nodestatemodel.h"

NodeStateView::NodeStateView(QWidget *parent) :
    QTableView {parent}
  , m_model(new NodeStateModel(this))
{
    setupContextMenu();
    setModel(m_model);
}

void NodeStateView::setProcessHolder(QSharedPointer<ProcessHolder> processHolder)
{
    m_processHolder = processHolder;
    connect(m_processHolder.data(), &ProcessHolder::stateChanged, m_model, &NodeStateModel::onNodeStateChanged);
}

NodeStateModel *NodeStateView::nodeModel() const
{
    return m_model;
}

void NodeStateView::setNodeFilter(const NodeFilter &nodeFilter)
{
    m_nodeFilter = nodeFilter;
    nodeModel()->setFilter(nodeFilter);
}

void NodeStateView::setupContextMenu()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    QAction *nodes = new QAction(tr("Manage"), this);
    QAction *refresh = new QAction(tr("Refresh"), this);

    connect(refresh, &QAction::triggered, this, [this]() ->void {
       if (NodeStateModel *nModel = dynamic_cast<NodeStateModel*>(model())) {
           nModel->refreshData();
       }
    });

    connect(nodes, &QAction::triggered, this, &NodeStateView::manageRequested);

    addAction(nodes);
    addAction(refresh);
}

void NodeStateView::processAddNodeDialog()
{
    AddNodeDialog dialog;
    dialog.setDefaultType(static_cast<int>(m_nodeFilter.type));
    dialog.setDefaultVendor(static_cast<int>(m_nodeFilter.vendor));
    connect(&dialog, &AddNodeDialog::addNodeRequested, this, [this] (int type, int vendor,
            const QString &name, const QString &dest) {
        m_processHolder->addNode(static_cast<Node::NodeType>(type),
                                 static_cast<Node::NodeVendor>(vendor),
                                 name, dest);
    });
    dialog.exec();
}
