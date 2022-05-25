#ifndef PROCESSHOLDER_H
#define PROCESSHOLDER_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QProcess>

#include <memory>

#include "types/node.h"

class QTimer;


class ProcessHolder final : public QObject
{
    Q_OBJECT
public:
    explicit ProcessHolder(QObject *parent = nullptr);
    ~ProcessHolder();
    void addNode(Node::NodeType, Node::NodeVendor, const QString &nodeName, const QString &nodeDestination);
    bool removeNode(Node::NodeType, const QString &nodeName);
signals:
    void stateChanged(const Node &node, Node::NodeState state);
private:
    void loadNodes();
    void saveNodes();
    void interrogate();
    void processPingOutput();
    void processPingFinished(int exitCode, QProcess::ExitStatus exitStatus);
private:
    QMap<Node, QProcess*> m_processes;
    QHash<QProcess*, QString> m_output;

    QVector<Node> m_nodes;

    QTimer *m_interrogatorTimer;
};

#endif // PROCESSHOLDER_H
