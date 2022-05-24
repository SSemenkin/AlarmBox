#ifndef NODESTATEVIEW_H
#define NODESTATEVIEW_H

#include <QTableView>

class ProcessHolder;

class NodeStateView : public QTableView
{
public:
    NodeStateView(QWidget *parent = nullptr);

    void setProcessHolder(QSharedPointer<ProcessHolder> processHolder);

private:
    void setupContextMenu();
    void processAddNodeDialog();
private:
    QSharedPointer<ProcessHolder> m_processHolder;
};

#endif // NODESTATEVIEW_H
