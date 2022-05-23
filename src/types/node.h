#ifndef NODE_H
#define NODE_H

#include <QString>

class Node
{
public:
    Node();

    enum class NodeType {
        Invalid = -1,
        Node_GSM,
        Node_UMTS,
        Node_LTE
    };

    enum class NodeVendor {
        Invalid = -1,
        Huawei,
        Ericssson
    };

    enum class NodeState {
        Working,
        Unavailable
    };

    NodeType nodeType() const;
    void setNodeType(NodeType type);

    NodeVendor nodeVendor() const;
    void setNodeVendor(NodeVendor vendor);

    QString nodeName() const;
    void setNodeName(const QString &nodeName);

    QString nodeDestination() const;
    void setNodeDestination(const QString &destination);

private:
    NodeType m_nodeType {NodeType::Invalid};
    NodeVendor m_nodeVendor {NodeVendor::Invalid};
    QString m_nodeName;
    QString m_destination;
};

#endif // NODE_H
