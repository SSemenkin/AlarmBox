#include "alarmdisplaywidget.h"
#include "ui_alarmdisplaywidget.h"

AlarmDisplayWidget::AlarmDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmDisplayWidget)
{
    ui->setupUi(this);
}

AlarmDisplayWidget::~AlarmDisplayWidget()
{
    delete ui;
}
