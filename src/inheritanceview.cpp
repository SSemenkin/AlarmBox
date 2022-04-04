#include "inheritanceview.h"
#include "ui_inheritanceview.h"

InheritanceView::InheritanceView(const QHash<Telnet *, QMap<QString, QString>> &objectsHierarchy, QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::InheritanceView)
{
    ui->setupUi(this);
    ui->treeWidget->setObjectsHierarchy(objectsHierarchy);
    ui->treeWidget->setHeaderLabels({tr("Objects")});

    connect(ui->inputLine, &QLineEdit::returnPressed, this, [this] () {
       ui->treeWidget->find(ui->inputLine->text());
    });
}

InheritanceView::~InheritanceView()
{
    delete ui;
}

InheritanceTreeWidget *InheritanceView::inheritanceTreeWidget() const
{
    return ui->treeWidget;
}
