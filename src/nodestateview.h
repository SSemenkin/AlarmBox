#ifndef NODESTATEVIEW_H
#define NODESTATEVIEW_H

#include <QTableView>
#include "nodestatemodel.h"

class ProcessHolder;

class NodeStateView : public QTableView
{
    Q_OBJECT
public:
    NodeStateView(QWidget *parent = nullptr);

    void setProcessHolder(QSharedPointer<ProcessHolder> processHolder);
    NodeStateModel* nodeModel() const;
    void setNodeFilter(const NodeFilter& nodeFilter);
signals:
    void manageRequested();
private:
    void setupContextMenu();
    void processAddNodeDialog();
private:
    QSharedPointer<ProcessHolder> m_processHolder;
    NodeStateModel *m_model;
    NodeFilter m_nodeFilter;
};

#endif // NODESTATEVIEW_H
