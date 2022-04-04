#include "inheritancetreewidget.h"
#include "network/telnet.h"

#include <QAction>
#include <QMessageBox>


InheritanceTreeWidget::InheritanceTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setupContextMenu();
}

InheritanceTreeWidget::InheritanceTreeWidget(const QHash<Telnet*, QMap<QString, QString>> &hierarchy,
                                             QWidget *parent) :
    QTreeWidget(parent)
  , m_hierarchy(hierarchy)
{
    buildTree();
    setupContextMenu();
}

QHash<Telnet *, QMap<QString, QString> > InheritanceTreeWidget::objectsHierarchy() const
{
    return m_hierarchy;
}

void InheritanceTreeWidget::setObjectsHierarchy(const QHash<Telnet*, QMap<QString, QString>> &hierarchy)
{
    if (m_hierarchy == hierarchy) {
        return;
    }
    m_hierarchy = hierarchy;
    buildTree();
}

void InheritanceTreeWidget::find(const QString &object)
{
    if (object == m_prevPattern) {
        goToNext();
    } else {
        markPreviousSearhItemLikeNormal();
        m_prevPattern = object;
        m_prevSelection.clear();
        for (int i = 0 ; i < topLevelItemCount(); ++i) {
            for (int j = 0; j < topLevelItem(i)->childCount(); ++j) {
                if (topLevelItem(i)->child(j)->text(0).contains(object, Qt::CaseInsensitive)) {
                    m_prevSelection.push_back(topLevelItem(i)->child(j));
                    markItemByBrush(topLevelItem(i)->child(j));
                }
            }
        }

        if (!m_prevSelection.isEmpty()) {
            m_current = m_prevSelection.first();
            scrollToItem(m_current);
            markItemByBrush(m_current, Qt::yellow);
        }
    }
}

void InheritanceTreeWidget::buildTree()
{
    for (auto it = m_hierarchy.begin(); it != m_hierarchy.end(); ++it) {

        QTreeWidgetItem *root = new QTreeWidgetItem(QStringList {it.key()->parsedTitle()} );
        addTopLevelItem(root);

        for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
            root->addChild(new TreeItem(QStringList {jt.value()}, it.key()->hostname() ));
        }
        expandItem(root);
    }
}

void InheritanceTreeWidget::cleanTree()
{
    m_current = nullptr;
    m_prevSelection.clear();
    m_prevPattern.clear();

    while(topLevelItemCount() > 0){
        while(topLevelItem(0)->childCount() > 0) {
            topLevelItem(0)->removeChild(0);
        }
        delete takeTopLevelItem(0);
    }
}

void InheritanceTreeWidget::markPreviousSearhItemLikeNormal()
{
    for (int i = 0; i < m_prevSelection.size(); ++i) {
        markItemByBrush(m_prevSelection.at(i), Qt::NoBrush);
    }
}

void InheritanceTreeWidget::markItemByBrush(QTreeWidgetItem *item, const QBrush &brush)
{
    for (int i = 0; i < item->columnCount(); ++i) {
        item->setBackground(0, brush);
    }
}

void InheritanceTreeWidget::goToNext()
{
    for (int i = 0; i < m_prevSelection.size(); ++i) {
        if (m_prevSelection.at(i) == m_current) {
            markItemByBrush(m_current);
            if (i + 1 < m_prevSelection.size()) {
                m_current = m_prevSelection[i + 1];
            } else {
                m_current = m_prevSelection.first();
            }
            scrollToItem(m_current);
            markItemByBrush(m_current, Qt::yellow);
            break;
        }
    }
}

void InheritanceTreeWidget::setupContextMenu()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *mblObject = new QAction(QIcon(":/icons/apper.svg"), tr("Manually block object"), this);
    connect(mblObject, &QAction::triggered, this, &InheritanceTreeWidget::deactivateRBS);
    addAction(mblObject);
}

void InheritanceTreeWidget::deactivateRBS()
{
    QList<QTreeWidgetItem*> d = selectedItems();

    if (d.isEmpty()) {
        return;
    }

    if (isTopLevelTreeItem(d.first())) {
        return;
    }

    QString object = d.first()->text(0);
    int result = QMessageBox::question(this, tr("Manually block"), tr("Are you sure you want to block ") +
                                       object + "?");

    TreeItem *treeItem = dynamic_cast<TreeItem*>(d.first());
    if (result == QMessageBox::Yes && treeItem){
        emit deactivateRBSRequested(object, treeItem->sourceHostname());
    }
}

bool InheritanceTreeWidget::isTopLevelTreeItem(QTreeWidgetItem *item)
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        if (topLevelItem(i) == item) {
            return true;
        }
    }
    return false;
}
