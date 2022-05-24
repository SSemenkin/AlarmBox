#include "node.h"

Node::Node()
{

}

Node::Node(const QString &name, const QString &destination, NodeType nodeType, NodeVendor vendor) :
    m_name(name)
  , m_destination(destination)
  , m_nodeType(nodeType)
  , m_nodeVendor(vendor)
{

}

Node::NodeType Node::nodeType() const
{
    return m_nodeType;
}

void Node::setNodeType(NodeType type)
{
    m_nodeType = type;
}

Node::NodeVendor Node::nodeVendor() const
{
    return m_nodeVendor;
}

void Node::setNodeVendor(NodeVendor vendor)
{
    m_nodeVendor = vendor;
}

QString Node::nodeName() const
{
    return m_name;
}

void Node::setNodeName(const QString &nodeName)
{
    m_name = nodeName;
}

QString Node::nodeDestination() const
{
    return m_destination;
}

void Node::setNodeDestination(const QString &destination)
{
    m_destination = destination;
}

QVariantMap Node::toVariantMap() const
{

    using std::make_pair;

    return {
            make_pair("name", m_name),
            make_pair("destination", m_destination),
            make_pair("nodeType", static_cast<int>(m_nodeType)),
            make_pair("nodeVendor", static_cast<int>(m_nodeVendor))
            };
}

// static
Node Node::fromVariantMap(const QVariantMap &nodeInfo)
{
    QString name = nodeInfo["name"].toString();
    QString dest = nodeInfo["destination"].toString();
    Node::NodeType type = static_cast<Node::NodeType>(nodeInfo["nodeType"].toInt());
    Node::NodeVendor vendor = static_cast<Node::NodeVendor>(nodeInfo["nodeVendor"].toInt());

    return Node(name, dest, type, vendor);
}

bool Node::operator ==(const Node &other) const
{
    return m_name == other.nodeName() &&
           m_nodeType == other.nodeType();
}
