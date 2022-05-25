#include "processholder.h"
#include "settings.h"

#include <QTimer>
#include <QTextCodec>
#include <QMessageBox>

ProcessHolder::ProcessHolder(QObject *parent)
    : QObject{parent}
    , m_interrogatorTimer(new QTimer(this))
{
    loadNodes();
    connect(m_interrogatorTimer, &QTimer::timeout, this, &ProcessHolder::interrogate);
    m_interrogatorTimer->start(60 * 1000);
}

ProcessHolder::~ProcessHolder()
{
    saveNodes();
}

void ProcessHolder::addNode(Node::NodeType type, Node::NodeVendor vendor, const QString &nodeName, const QString &nodeDestination)
{
    Node node(nodeName, nodeDestination, type, vendor);
    if (!m_nodes.contains(node)) {
        m_nodes.push_back(node);

        QProcess* pingProcess = new QProcess(this);
        pingProcess->setProgram("ping");
        pingProcess->setArguments({node.nodeDestination()});
        pingProcess->setProcessChannelMode(QProcess::ProcessChannelMode::SeparateChannels);

        connect(pingProcess, &QProcess::readyRead, this, &ProcessHolder::processPingOutput);
        connect(pingProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                this, &ProcessHolder::processPingFinished);

        m_processes[m_nodes.last()] = pingProcess;
    } else {
        QMessageBox::information(nullptr, tr("Error"), tr("Node is already exists."));
    }
}

bool ProcessHolder::removeNode(Node::NodeType type, const QString &nodeName)
{
    Node item(nodeName, "...", type, Node::NodeVendor::Huawei);
    auto place = std::find(m_nodes.begin(), m_nodes.end(), item);

    if (place == m_nodes.end()) {
        return false;
    } else {
        m_nodes.erase(place);
        return true;
    }
}

void ProcessHolder::loadNodes()
{
    QVector<Node> nodes = Settings::instance()->getNodes();

    for (const Node &node : qAsConst(nodes)) {
        addNode(node.nodeType(),
                node.nodeVendor(),
                node.nodeName(),
                node.nodeDestination());
    }
    interrogate();
}

void ProcessHolder::saveNodes()
{
    Settings::instance()->setNodeList(m_nodes);
}

void ProcessHolder::interrogate()
{
    for (auto it = m_processes.begin(); it != m_processes.end(); ++it) {
        it.value()->start();
    }
}

void ProcessHolder::processPingOutput()
{
    QProcess *process = qobject_cast<QProcess*>(sender());

    if (process) {
        QByteArray dataFromProcess = process->readAll();
        char *data = dataFromProcess.data();
        QTextCodec *windowsCmdCodec = QTextCodec::codecForName("cp-866");
        m_output[process].append(windowsCmdCodec->toUnicode(data));
    }
}

void ProcessHolder::processPingFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << exitCode
             << exitStatus;
    // Тут нужно подвести итог по полученным данным.
    QProcess* process = qobject_cast<QProcess*>(sender());

    if (!process) {
        return;
    }

    QString output = m_output.value(process);
    Node node = m_processes.key(process);



    if (output.contains("(0%") && output.count("TTL") > 2) {
        emit stateChanged(node, Node::NodeState::Working);
    } else {
        emit stateChanged(node, Node::NodeState::Unavailable);
    }
}
