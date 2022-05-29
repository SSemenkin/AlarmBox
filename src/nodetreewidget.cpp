#include "nodetreewidget.h"
#include "processholder.h"

#include <QAction>
#include <QMessageBox>

NodeTreeWidget::NodeTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
  , m_nodes(ProcessHolder::instance()->nodes())
{
    setupContextMenu();
}

void NodeTreeWidget::buildTree()
{
    setColumnCount(3);
    setHeaderLabels({tr("Node name"), tr("Node destination"), tr("Node Vendor")});
    addTopLevelItem(new QTreeWidgetItem({"LTE"}));
    addTopLevelItem(new QTreeWidgetItem({"UMTS"}));
    addTopLevelItem(new QTreeWidgetItem({"GSM"}));

    for (const Node &node : qAsConst(m_nodes)) {
        switch(node.nodeType()) {
        case Node::NodeType::Node_GSM:
            topLevelItem(2)->addChild(makeNodeItem(node));
        break;
        case Node::NodeType::Node_LTE:
            topLevelItem(0)->addChild(makeNodeItem(node));
        break;
        case Node::NodeType::Node_UMTS:
            topLevelItem(1)->addChild(makeNodeItem(node));
        break;
        case Node::NodeType::Invalid:
            // for disable warning
        break;
        }
    }

    expandAll();

    for (int i = 0; i < colorCount(); ++i) {
        resizeColumnToContents(i);
    }
}

QTreeWidgetItem* NodeTreeWidget::makeNodeItem(const Node &node) const
{
    QString nodeVendor = node.nodeVendor() == Node::NodeVendor::Ericsson ? "Ericsson" : "Huawei";
    return new QTreeWidgetItem({node.nodeName(), node.nodeDestination(), nodeVendor});
}

void NodeTreeWidget::removeNode()
{
    if (!isTopLevelItemSelected()) {
        const auto items = selectedItems();

        QTreeWidgetItem *nodeItem = items.first();
        QTreeWidgetItem *parentItem = nodeItem->parent();

        Node::NodeVendor vendor = nodeItem->text(2) == "Ericsson" ? Node::NodeVendor::Ericsson : Node::NodeVendor::Huawei;
        Node::NodeType type = parentItem->text(0) == "LTE" ? Node::NodeType::Node_LTE : parentItem->text(0) == "UMTS" ?
                                                             Node::NodeType::Node_UMTS : Node::NodeType::Node_GSM;

        Node node (nodeItem->text(0), nodeItem->text(1), type, vendor);

        bool removeResult = ProcessHolder::instance()->nodes().removeOne(node);

        if (removeResult) {
            parentItem->removeChild(nodeItem);
            QMessageBox::information(this, tr("Node removed"), tr("Node successfully removed."));
        } else {
            QMessageBox::warning(this, tr("Node not removed"), tr("Node cannot be removed."));
        }

    }
}

void NodeTreeWidget::setupContextMenu()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *remove = new QAction(QIcon(":/icons/com.azuredatastudio.oss.svg"), tr("&Remove"));
    connect(remove, &QAction::triggered, this, &NodeTreeWidget::removeNode);
    addAction(remove);
}

bool NodeTreeWidget::isTopLevelItemSelected() const
{
    const auto items = selectedItems();

    for (int i = 0; i < topLevelItemCount(); ++i) {
        if (items.contains(topLevelItem(i))) {
            return true;
        }
    }
    return false;
}
