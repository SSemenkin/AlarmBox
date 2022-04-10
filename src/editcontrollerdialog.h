#ifndef EDITCONTROLLERDIALOG_H
#define EDITCONTROLLERDIALOG_H

#include <QDialog>

class Telnet;
class ControllerEditWidget;

namespace Ui {
class EditControllerDialog;
}

class EditControllerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditControllerDialog(QSharedPointer<Telnet> telnet, QWidget *parent = nullptr);
    ~EditControllerDialog();

private:
    void onAcceptPressed();
private:
    Ui::EditControllerDialog *m_ui;
    ControllerEditWidget *m_d;
    QSharedPointer<Telnet> m_telnet;
};

#endif // EDITCONTROLLERDIALOG_H
