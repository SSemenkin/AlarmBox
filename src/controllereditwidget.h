#ifndef CONTROLLEREDITWIDGET_H
#define CONTROLLEREDITWIDGET_H

#include <QWidget>

class Telnet;
namespace Ui {
class ControllerEditWidget;
}

class ControllerEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerEditWidget(QSharedPointer<Telnet> telnet, QWidget *parent = nullptr);
    explicit ControllerEditWidget(QWidget *parent = nullptr);
    ~ControllerEditWidget();

    QString hostname() const;
    QString username() const;
    QString password() const;

private:
    Ui::ControllerEditWidget *ui;
};

#endif // CONTROLLEREDITWIDGET_H
