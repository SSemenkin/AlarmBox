#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QVariantMap>

class Node
{
public:

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

    Node();
    Node(const QString &name, const QString &destination, Node::NodeType nodeType, Node::NodeVendor nodeVendor);

    NodeType nodeType() const;
    void setNodeType(NodeType type);

    NodeVendor nodeVendor() const;
    void setNodeVendor(NodeVendor vendor);

    QString nodeName() const;
    void setNodeName(const QString &nodeName);

    QString nodeDestination() const;
    void setNodeDestination(const QString &destination);

    QVariantMap toVariantMap() const;
    static Node fromVariantMap(const QVariantMap &nodeInfo);

private:
    QString m_name;
    QString m_destination;
    NodeType m_nodeType {NodeType::Invalid};
    NodeVendor m_nodeVendor {NodeVendor::Invalid};
};

#endif // NODE_H
