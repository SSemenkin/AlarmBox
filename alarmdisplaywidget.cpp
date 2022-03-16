#include "alarmdisplaywidget.h"
#include "ui_alarmdisplaywidget.h"


AlarmDisplayWidget::AlarmDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmDisplayWidget)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setHeaderLabels(QStringList {tr("Object"), tr("Alarm"), tr("Controller"), tr("Raised Time"), tr("Comment")});
    connect(ui->treeWidget, &AlarmTreeWidget::refresh, this, &AlarmDisplayWidget::refreshRequested);
}

AlarmDisplayWidget::~AlarmDisplayWidget()
{
    delete ui;
}

void AlarmDisplayWidget::processAlarms(const QVector<Alarm> &alarms)
{
    ui->treeWidget->processAlarms(alarms);
}

void AlarmDisplayWidget::onControllerChanged(const QString &hostname)
{
    ui->treeWidget->onCurrentControllerChanged(hostname);
}

void AlarmDisplayWidget::retranslate()
{
    ui->retranslateUi(this);
}

