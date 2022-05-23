#ifndef PROCESSHOLDER_H
#define PROCESSHOLDER_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include <memory>

#include "types/node.h"

class QProcess;
class QTimer;


class ProcessHolder final : public QObject
{
    Q_OBJECT
public:
    explicit ProcessHolder(QObject *parent = nullptr);
    void addNode(Node::NodeType, Node::NodeVendor, const QString &nodeName, const QString &nodeDestination);
    bool removeNode(Node::NodeType, const QString &nodeName);
signals:
    void stateChanged(QSharedPointer<Node>, Node::NodeState state);
private:
    QMap<Node*, QSharedPointer<QProcess>> m_processes;
    QMap<Node*, Node::NodeState> m_nodeStates;

    QVector<QSharedPointer<Node*>> m_nodes;

    QTimer *m_interrogatorTimer;
};

#endif // PROCESSHOLDER_H
