#include "controllersedit.h"
#include "ui_controllersedit.h"

ControllersEdit::ControllersEdit(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ControllersEdit)
{
    m_ui->setupUi(this);
}

ControllersEdit::~ControllersEdit()
{
    delete m_ui;
}

QPushButton *ControllersEdit::addControllerButton() const
{
    return m_ui->addController;
}

QPushButton *ControllersEdit::editControllerButton() const
{
    return m_ui->editController;
}

QPushButton *ControllersEdit::removeControllerButton() const
{
    return m_ui->removeController;
}

ControllerListWidget *ControllersEdit::controllerWidget() const
{
    return m_ui->controllerWidget;
}
