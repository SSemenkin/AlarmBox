#include "nodestatemodel.h"

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
    return 4;
}

QVariant NodeStateModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole ||
        role == Qt::EditRole) {

        switch(index.column()) {
        case 0:
            return m_nodes.at(index.row()).m_source->nodeName();
        break;
        case 1:
            return m_nodes.at(index.row()).m_source->nodeDestination();
            break;
        case 2:
            return m_nodes.at(index.row()).m_state == Node::NodeState::Unavailable ? "Unavailable" : "Works";
            break;
        case 3:
            return "";
            break;
        default:
            return "";
            break;
        }

    } else {
        return QVariant();
    }
}
