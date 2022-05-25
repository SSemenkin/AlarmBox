#include "nodestatemodel.h"
#include <QDebug>
NodeStateModel::NodeStateModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int NodeStateModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_nodes.size();
}

int NodeStateModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant NodeStateModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole ||
        role == Qt::EditRole) {

        switch(index.column()) {
        case 0:
            return m_nodes.at(index.row()).m_source.nodeName();
        break;
        case 1:
            return m_nodes.at(index.row()).m_source.nodeDestination();
            break;
        case 2:
            return m_nodes.at(index.row()).m_state == Node::NodeState::Unavailable ? tr("Remote host unreachable")
                                                                                   : tr("Works");
            break;
        default:
            return "";
            break;
        }
    } else {
        return QVariant();
    }
}

QVariant NodeStateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
       static QStringList headers {tr("Node name"), tr("Destination"), tr("State")};
       return headers.at(section);
   } else {
       return QAbstractTableModel::headerData(section, orientation, role);
   }
}

void NodeStateModel::onNodeStateChanged(const Node &node, Node::NodeState state)
{
    if (m_nodeFilter.type == node.nodeType() && m_nodeFilter.vendor == node.nodeVendor()) {
        auto duplicate = std::find_if(m_nodes.begin(), m_nodes.end(), [node](const NodeStateItem item) {
            return node == item.m_source;
        });

        if (duplicate == m_nodes.end()) {
            m_nodes.push_back({node, state});
        } else {
            if (duplicate->m_state != state) {
                duplicate->m_state = state;
            }
        }
    } // filtering
}

void NodeStateModel::setFilter(NodeFilter filter)
{
    m_nodeFilter = filter;
}

void NodeStateModel::refreshData()
{
    beginResetModel();
    endResetModel();
}
