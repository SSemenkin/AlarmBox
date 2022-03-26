#ifndef CONTROLLERSEDIT_H
#define CONTROLLERSEDIT_H

#include <QWidget>

class QPushButton;
class ControllerListWidget;

namespace Ui {
class ControllersEdit;
}

class ControllersEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ControllersEdit(QWidget *parent = nullptr);
    ~ControllersEdit();

    QPushButton* addControllerButton() const;
    QPushButton* editControllerButton() const;
    QPushButton* removeControllerButton() const;

    ControllerListWidget* controllerWidget() const;
private:
    Ui::ControllersEdit *ui;
};

#endif // CONTROLLERSEDIT_H
