#include "controllersedit.h"
#include "ui_controllersedit.h"

ControllersEdit::ControllersEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllersEdit)
{
    ui->setupUi(this);
}

ControllersEdit::~ControllersEdit()
{
    delete ui;
}

QPushButton *ControllersEdit::addControllerButton() const
{
    return ui->addController;
}

QPushButton *ControllersEdit::editControllerButton() const
{
    return ui->editController;
}

QPushButton *ControllersEdit::removeControllerButton() const
{
    return ui->removeController;
}

ControllerListWidget *ControllersEdit::controllerWidget() const
{
    return ui->controllerWidget;
}
