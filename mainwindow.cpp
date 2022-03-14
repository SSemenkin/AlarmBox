#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->addController, &QPushButton::clicked,
            this, &MainWindow::execAddControllerDialog);
    connect(ui->editController, &QPushButton::clicked,
            this, &MainWindow::execEditControllerDialog);
    connect(ui->removeController, &QPushButton::clicked,
            ui->controllerWidget, &ControllerListWidget::removeController);

    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationFailed,
            ui->controllerWidget, &ControllerListWidget::processFailedControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationSuccessfull,
            ui->controllerWidget, &ControllerListWidget::processSuccessfullControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerInfo, this, [this] (QSharedPointer<Telnet> controller){
            qDebug() << ControllerInfo(controller);
    });


    connect(ui->controllerWidget, QOverload<Telnet*>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<Telnet*>::of(&ControllerOwnership::reconnect));
    connect(ui->controllerWidget, QOverload<const QString&>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::reconnect));

    connect(ui->controllerWidget, &ControllerListWidget::editRequested, this, &MainWindow::execEditControllerDialog);
    connect(ui->controllerWidget, &ControllerListWidget::removeRequested,
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::removeController));
    connect(ui->controllerWidget, &ControllerListWidget::detailRequested,
            &m_controllerOwner, &ControllerOwnership::onDetailControllerRequested);

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
    QList<QListWidgetItem*> d_items = ui->controllerWidget->selectedItems();
    if (d_items.isEmpty() || d_items.size() > 1) {
        QMessageBox::information(this, tr("Select more than one item"), tr("Please select only one item"));
        return;
    }
    QSharedPointer<Telnet> controller = m_controllerOwner.controller(d_items.first()->text());
    EditControllerDialog dialog(controller);
    dialog.exec();
}
