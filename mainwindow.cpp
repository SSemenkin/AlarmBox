#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controllerlistwidget.h"

#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTreeWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      controllersEdit(new ControllersEdit(this))
{
    ui->setupUi(this);

    connect(controllersEdit->addControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execAddControllerDialog);
    connect(controllersEdit->editControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execEditControllerDialog);
    connect(controllersEdit->removeControllerButton(), &QPushButton::clicked,
            controllersEdit->controllerWidget(), &ControllerListWidget::removeController);

    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationFailed,
            controllersEdit->controllerWidget(), &ControllerListWidget::processFailedControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationSuccessfull,
            controllersEdit->controllerWidget(), &ControllerListWidget::processSuccessfullControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerInfo, this, [] (QSharedPointer<Telnet> controller){
            qDebug() << ControllerInfo(controller);
    });


    connect(controllersEdit->controllerWidget(), QOverload<Telnet*>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<Telnet*>::of(&ControllerOwnership::reconnect));
    connect(controllersEdit->controllerWidget(), QOverload<const QString&>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::reconnect));

    connect(controllersEdit->controllerWidget(), &ControllerListWidget::editRequested, this, &MainWindow::execEditControllerDialog);
    connect(controllersEdit->controllerWidget(), &ControllerListWidget::removeRequested,
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::removeController));
    connect(controllersEdit->controllerWidget(), &ControllerListWidget::detailRequested,
            &m_controllerOwner, &ControllerOwnership::onDetailControllerRequested);

    QSplitter *splitter = new QSplitter(Qt::Orientation::Horizontal, this);
    splitter->addWidget(controllersEdit);
    splitter->addWidget(new QTreeWidget(this));
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
    QList<QListWidgetItem*> d_items = controllersEdit->controllerWidget()->selectedItems();
    if (d_items.isEmpty() || d_items.size() > 1) {
        QMessageBox::information(this, tr("Select more than one item"), tr("Please select only one item"));
        return;
    }
    QSharedPointer<Telnet> controller = m_controllerOwner.controller(d_items.first()->text());
    EditControllerDialog dialog(controller);
    dialog.exec();
}
