#ifndef NODEEDITDIALOG_H
#define NODEEDITDIALOG_H

#include <QDialog>

namespace Ui {
class NodeEditDialog;
}

class NodeEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NodeEditDialog(QWidget *parent = nullptr);
    ~NodeEditDialog();

private:
    Ui::NodeEditDialog *m_ui;
};

#endif // NODEEDITDIALOG_H
