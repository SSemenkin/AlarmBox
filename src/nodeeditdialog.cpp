#include "nodeeditdialog.h"
#include "ui_nodeeditdialog.h"

NodeEditDialog::NodeEditDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NodeEditDialog)
{
    m_ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    m_ui->treeWidget->buildTree();
}

NodeEditDialog::~NodeEditDialog()
{
    delete m_ui;
}
