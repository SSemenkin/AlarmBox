#ifndef NODESTATEVIEW_H
#define NODESTATEVIEW_H

#include <QTableView>

class NodeStateView : public QTableView
{
public:
    NodeStateView(QWidget *parent = nullptr);

private:
    void setupContextMenu();
};

#endif // NODESTATEVIEW_H
