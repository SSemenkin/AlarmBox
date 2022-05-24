#ifndef NODESTATEMODEL_H
#define NODESTATEMODEL_H

#include <QAbstractTableModel>
#include "types/node.h"

struct NodeStateItem
{
    Node *m_source;
    Node::NodeState m_state;
};

struct NodeFilter
{
    Node::NodeType type;
    Node::NodeVendor vendor;
};

class NodeStateModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NodeStateModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    //

    void onNodeStateChanged(Node*, Node::NodeState state);
    void setFilter(NodeFilter filter);
signals:

private:
    QVector<NodeStateItem> m_nodes;
    NodeFilter m_nodeFilter;
};

#endif // NODESTATEMODEL_H
