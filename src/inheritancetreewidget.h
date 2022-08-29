#ifndef INHERITANCETREEWIDGET_H
#define INHERITANCETREEWIDGET_H

#include "rbslocation.h"
#include "settings.h"
#include <QTreeWidget>
#include <QMap>


class TreeItem : public QTreeWidgetItem {
public:
    TreeItem(const QStringList &labels, const QString &sourceHostname) :
        QTreeWidgetItem(labels)
      , m_sourceHostname(sourceHostname)
    {

    }

    QString sourceHostname() const {
        return m_sourceHostname;
    }

private:
    QString m_sourceHostname;
};

class Telnet;

class InheritanceTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    InheritanceTreeWidget(QWidget *parent = nullptr);
    InheritanceTreeWidget(const QHash<Telnet*, QMap<QString, QString>> &hierarchy,
                          QWidget *parent = nullptr);

    QHash<Telnet*, QMap<QString ,QString>> objectsHierarchy() const;
    void setObjectsHierarchy(const QHash<Telnet*, QMap<QString, QString>> &hierarchy);

    void find(const QString &object);
signals:
    void deactivateRBSRequested(const QString &object, const QString &controllerHostname);
private:
    void buildTree();
    void cleanTree();
    void markPreviousSearhItemLikeNormal();
    void markItemByBrush(QTreeWidgetItem *item, const QBrush& brush = Qt::lightGray);
    void goToNext();
    void setupContextMenu();
    void deactivateRBS();
    bool isTopLevelTreeItem(QTreeWidgetItem *item);
    void showRBSLocation();
private:
    QHash<Telnet*, QMap<QString, QString>> m_hierarchy;
    RbsLocation m_location;

    QVector<QTreeWidgetItem *> m_prevSelection;
    QTreeWidgetItem *m_current {nullptr};
    QString m_prevPattern;
};

#endif // INHERITANCETREEWIDGET_H
