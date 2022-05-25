#ifndef NODEINFOMODEL_H
#define NODEINFOMODEL_H

#include <QAbstractTableModel>

class Node;
class ProcessHolder;

class NodeInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NodeInfoModel(QSharedPointer<ProcessHolder> holder, QObject *parent = nullptr);


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;


private:
    QVector<Node> &m_nodes;
};

#endif // NODEINFOMODEL_H
