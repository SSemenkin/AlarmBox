#ifndef NODESTATEMODEL_H
#define NODESTATEMODEL_H

#include <QAbstractTableModel>

class NodeStateModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NodeStateModel(QObject *parent = nullptr);

signals:

};

#endif // NODESTATEMODEL_H
