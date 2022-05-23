#ifndef ADDNODEDIALOG_H
#define ADDNODEDIALOG_H

#include <QDialog>

namespace Ui {
class AddNodeDialog;
}

class AddNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNodeDialog(QWidget *parent = nullptr);
    ~AddNodeDialog();

    void setDefaultType(int);
    void setDefaultVendor(int);

private:
    Ui::AddNodeDialog *ui;
};

#endif // ADDNODEDIALOG_H
