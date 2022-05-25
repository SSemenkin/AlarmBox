#include "nodeinfomodel.h"
#include "types/node.h"
#include "processholder.h"

NodeInfoModel::NodeInfoModel(QSharedPointer<ProcessHolder> holder, QObject *parent) :
    QAbstractTableModel(parent)
  , m_nodes(holder->nodes())
{

}

int NodeInfoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_nodes.size();
}

int NodeInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_nodes.isEmpty() ? 0 : 4;
}

QVariant NodeInfoModel::data(const QModelIndex &index, int role) const
{
    if (role  == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()) {
        case 0:
            return m_nodes.at(index.row()).nodeName();
            break;
        case 1:
            return m_nodes.at(index.row()).nodeDestination();
            break;
            case 2:
            {
                switch(m_nodes.at(index.row()).nodeType()) {
                case Node::NodeType::Node_GSM:
                    return "GSM";
                case Node::NodeType::Node_LTE:
                    return "LTE";
                case Node::NodeType::Node_UMTS:
                    return "UMTS";
                case Node::NodeType::Invalid:
                    return "Unknown";
                }

                break;
            }
            case 3:
            {
                switch(m_nodes.at(index.row()).nodeVendor()) {
                case Node::NodeVendor::Invalid:
                    return "Unknown";
                case Node::NodeVendor::Ericsson:
                    return "Ericsson";
                case Node::NodeVendor::Huawei:
                    return "Huawei";
                }

                break;
            }
        default:
            return QVariant();
        }
    } else {
        return QVariant();
    }
}
