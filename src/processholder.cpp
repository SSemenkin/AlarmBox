#include "processholder.h"

#include <QProcess>
#include <QTimer>

ProcessHolder::ProcessHolder(QObject *parent)
    : QObject{parent}
{

}

void ProcessHolder::addNode(Node::NodeType, Node::NodeVendor, const QString &nodeName, const QString &nodeDestination)
{

}

bool ProcessHolder::removeNode(Node::NodeType, const QString &nodeName)
{

}
