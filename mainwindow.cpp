#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controllerlistwidget.h"
#include "alarmdisplaywidget.h"
#include "settingsdialog.h"

#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QPropertyAnimation>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controllersEdit(new ControllersEdit(this))
    , m_alarmDisplayWidget(new AlarmDisplayWidget(this))
    , m_interrogator(new AlarmInterrogator(m_controllerOwner.controllerList()))
{
    ui->setupUi(this);

    connect(m_controllersEdit->addControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execAddControllerDialog);
    connect(m_controllersEdit->editControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execEditControllerDialog);
    connect(m_controllersEdit->removeControllerButton(), &QPushButton::clicked,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::removeController);

    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationFailed,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processFailedControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationSuccessfull,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processSuccessfullControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerInfo, this, [] (QSharedPointer<Telnet> controller){
            qDebug() << ControllerInfo(controller);
    });
    connect(&m_controllerOwner, &ControllerOwnership::controllerAdded,
            m_interrogator.data(), &AlarmInterrogator::onControllerAdded);
    connect(&m_controllerOwner, &ControllerOwnership::controllerRemoved,
            m_interrogator.data(), &AlarmInterrogator::onControllerRemoved);


    connect(m_controllersEdit->controllerWidget(), QOverload<Telnet*>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<Telnet*>::of(&ControllerOwnership::reconnect));
    connect(m_controllersEdit->controllerWidget(), QOverload<const QString&>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::reconnect));

    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::editRequested, this, &MainWindow::execEditControllerDialog);
    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::removeRequested,
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::removeController));
    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::detailRequested,
            &m_controllerOwner, &ControllerOwnership::onDetailControllerRequested);
    connect(ui->action_Settings, &QAction::triggered, this, &MainWindow::execSettingsDialog);

    connect(m_interrogator.data(), &AlarmInterrogator::alarmsReceived,
            m_alarmDisplayWidget, &AlarmDisplayWidget::processAlarms);


    QSplitter *splitter = new QSplitter(Qt::Orientation::Horizontal, this);
    splitter->addWidget(m_controllersEdit);
    splitter->addWidget(m_alarmDisplayWidget);
    ui->horizontalLayout->addWidget(splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::execAddControllerDialog()
{
    AddControllerDialog dialog;
    connect(&dialog, &AddControllerDialog::newController, &m_controllerOwner, &ControllerOwnership::addController);
    dialog.exec();
}

void MainWindow::execEditControllerDialog()
{
    QList<QListWidgetItem*> d_items = m_controllersEdit->controllerWidget()->selectedItems();
    if (d_items.isEmpty() || d_items.size() > 1) {
        QMessageBox::information(this, tr("Select more than one item"), tr("Please select only one item"));
        return;
    }
    QSharedPointer<Telnet> controller = m_controllerOwner.controller(d_items.first()->text());
    EditControllerDialog dialog(controller);
    dialog.exec();
}

void MainWindow::execSettingsDialog()
{
    SettingsDialog dialog;
    dialog.exec();
}
