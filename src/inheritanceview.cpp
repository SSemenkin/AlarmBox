#include "inheritanceview.h"
#include "ui_inheritanceview.h"

InheritanceView::InheritanceView(const QHash<Telnet *, QMap<QString, QString>> &objectsHierarchy, QWidget *parent) :
    QWidget(parent)
  , m_ui(new Ui::InheritanceView)
{
    m_ui->setupUi(this);
    m_ui->treeWidget->setObjectsHierarchy(objectsHierarchy);
    m_ui->treeWidget->setHeaderLabels({tr("Objects")});

    connect(m_ui->inputLine, &QLineEdit::returnPressed, this, [this] () {
       m_ui->treeWidget->find(m_ui->inputLine->text());
    });
}

InheritanceView::~InheritanceView()
{
    delete m_ui;
}

InheritanceTreeWidget *InheritanceView::inheritanceTreeWidget() const
{
    return m_ui->treeWidget;
}
