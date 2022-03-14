#ifndef ADDCONTROLLERDIALOG_H
#define ADDCONTROLLERDIALOG_H

#include <QDialog>

class ControllerEditWidget;

namespace Ui {
class AddControllerDialog;
}

class AddControllerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddControllerDialog(QWidget *parent = nullptr);
    ~AddControllerDialog();

signals:
    void newController(const QString &hostname,
                       const QString &username,
                       const QString &password);
private:
    void onAcceptPressed();

private:
    Ui::AddControllerDialog *ui;
    ControllerEditWidget* m_d;
};

#endif // ADDCONTROLLERDIALOG_H
