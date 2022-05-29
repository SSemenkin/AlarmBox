#ifndef NODETREEWIDGET_H
#define NODETREEWIDGET_H

#include <QTreeWidget>
#include <QObject>

class Node;

class NodeTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    NodeTreeWidget(QWidget *parent = nullptr);

private:
    void buildTree();
    QTreeWidgetItem* makeNodeItem(const Node &node) const;
    void removeNode();
    void setupContextMenu();
    bool isTopLevelItemSelected() const;
private:
    QVector<Node> &m_nodes;

    friend class NodeEditDialog;
};

#endif // NODETREEWIDGET_H
