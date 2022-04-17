#include "alarmdisplaywidget.h"
#include "ui_alarmdisplaywidget.h"


AlarmDisplayWidget::AlarmDisplayWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AlarmDisplayWidget)
{
    m_ui->setupUi(this);

    m_ui->treeWidget->setColumnCount(6);
    m_ui->treeWidget->setHeaderLabels(QStringList {tr("Object"), tr("Alarm"), tr("Controller"),
                                                 tr("Comment"), tr("Raised Time"), tr("Cleared Time")});
    connect(m_ui->treeWidget, &AlarmTreeWidget::refresh, this, &AlarmDisplayWidget::refreshRequested);

    m_ui->treeWidget->sortItems(0, Qt::SortOrder::AscendingOrder);
}

AlarmDisplayWidget::~AlarmDisplayWidget()
{
    delete m_ui;
}

void AlarmDisplayWidget::processAlarms(const QVector<Alarm> &alarms)
{
    m_ui->treeWidget->processAlarms(alarms);
}

void AlarmDisplayWidget::onControllerChanged(const QString &hostname)
{
    m_ui->treeWidget->onCurrentControllerChanged(hostname);
}

const AlarmTreeWidget *AlarmDisplayWidget::alarmTreeWidget() const
{
    return m_ui->treeWidget;
}
