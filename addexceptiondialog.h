#ifndef ADDEXCEPTIONDIALOG_H
#define ADDEXCEPTIONDIALOG_H

#include <QDialog>

class Telnet;
class Settings;

namespace Ui {
class AddExceptionDialog;
}

class AddExceptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddExceptionDialog(const QList<QSharedPointer<Telnet>> &controllers, QWidget *parent = nullptr);
    ~AddExceptionDialog();
    void accept() override;

signals:
    void exceptionAdded(const QString &controller,
                        const QString &object,
                        const QString &alarmType);
private:
    Ui::AddExceptionDialog *ui;
    const QList<QSharedPointer<Telnet>> &m_controllers;
    Settings *m_settings;
};

#endif // ADDEXCEPTIONDIALOG_H
