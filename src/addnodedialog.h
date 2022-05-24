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
    virtual void accept() override;

signals:
    void addNodeRequested(int type,
                          int vendor,
                          const QString &name,
                          const QString &dest);

private:
    Ui::AddNodeDialog *ui;
};

#endif // ADDNODEDIALOG_H
