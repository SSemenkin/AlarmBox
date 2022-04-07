#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controllerlistwidget.h"
#include "alarmdisplaywidget.h"
#include "settingsdialog.h"
#include "alarmtreewidget.h"

#include "inheritanceview.h"
#include "inheritancetreewidget.h"

#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTranslator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_interrogator(new AlarmInterrogator(m_controllerOwner.controllerList()))
    , m_splitter(new QSplitter(Qt::Horizontal))
    , m_controllersEdit(new ControllersEdit(this))
    , m_alarmDisplayWidget(new AlarmDisplayWidget(this))
{
    ui->setupUi(this);

    ///Buttons
    connect(m_controllersEdit->addControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execAddControllerDialog);
    connect(m_controllersEdit->editControllerButton(), &QPushButton::clicked,
            this, &MainWindow::execEditControllerDialog);
    connect(m_controllersEdit->removeControllerButton(), &QPushButton::clicked,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::removeController);

    ///

    ///ControllerOwner
    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationFailed,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processFailedControllerAuthentication);
    connect(&m_controllerOwner, &ControllerOwnership::controllerAuthenticationSuccessfull,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processSuccessfullControllerAuthentication);

    connect(&m_controllerOwner, &ControllerOwnership::controllerInfo, this, [] (QSharedPointer<Telnet> controller){
        Q_UNUSED(controller);
            //qDebug() << ControllerInfo(controller);
    });

    connect(&m_controllerOwner, &ControllerOwnership::controllerAdded,
            m_interrogator.data(), &AlarmInterrogator::onControllerAdded);
    connect(&m_controllerOwner, &ControllerOwnership::controllerRemoved,
            m_interrogator.data(), &AlarmInterrogator::onControllerRemoved);

    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::addRequested, this, &MainWindow::execAddControllerDialog);
    ///


    /// ControllerListWidget
    connect(m_controllersEdit->controllerWidget(), QOverload<Telnet*>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<Telnet*>::of(&ControllerOwnership::reconnect));
    connect(m_controllersEdit->controllerWidget(), QOverload<const QString&>::of(&ControllerListWidget::reconnectRequested),
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::reconnect));
    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::controllerSelectionChanged,
            m_alarmDisplayWidget, &AlarmDisplayWidget::onControllerChanged);

    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::editRequested, this, &MainWindow::execEditControllerDialog);
    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::removeRequested,
            &m_controllerOwner, QOverload<const QString&>::of(&ControllerOwnership::removeController));
    connect(m_controllersEdit->controllerWidget(), &ControllerListWidget::detailRequested,
            &m_controllerOwner, &ControllerOwnership::onDetailControllerRequested);
    ///

    /// AlarmDisplayWidget
    connect(m_alarmDisplayWidget->alarmTreeWidget(), &AlarmTreeWidget::updated, this, [this] () {
        ui->statusbar->showMessage(tr("Last update : ") + QDateTime::currentDateTime().toString(Qt::DateFormat::LocaleDate));
    });

    connect(m_alarmDisplayWidget->alarmTreeWidget(), &AlarmTreeWidget::activateRBSRequested,
            m_interrogator.data(), &AlarmInterrogator::onActivateRBSRequested);    
    connect(m_alarmDisplayWidget, &AlarmDisplayWidget::refreshRequested,
            m_interrogator.data(), &AlarmInterrogator::interrogateControllers);

    ///

    /// Action
    connect(ui->action_Settings, &QAction::triggered, this, &MainWindow::execSettingsDialog);
    connect(ui->actionAbout_program, &QAction::triggered, this, &MainWindow::aboutProgram);

    ///

    ///  Interrogator
    connect(m_interrogator.data(), &AlarmInterrogator::alarmsReceived,
            m_alarmDisplayWidget, &AlarmDisplayWidget::processAlarms);

    connect(m_interrogator.data(), &AlarmInterrogator::noMMLError,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processControllerNoMMLError);
    connect(m_interrogator.data(), &AlarmInterrogator::MMLError,
            m_controllersEdit->controllerWidget(), &ControllerListWidget::processControllerMMLError);

    connect(m_interrogator.data(), &AlarmInterrogator::hierarchyUpdated, this, [this] () {
        auto hierarchy = m_interrogator.data()->objectsHierarchy();
        InheritanceView *view = new InheritanceView(hierarchy, this);
        m_splitter->addWidget(view);

        QList<int> sizes = Settings::instance()->getSplitterSizes();
        if(!sizes.isEmpty()) {
            m_splitter->setSizes(sizes);
        }

        connect(view->inheritanceTreeWidget(), &InheritanceTreeWidget::deactivateRBSRequested,
                m_interrogator.data(), &AlarmInterrogator::onDeactivateRBSRequested);
    });

    ///

    createSplitter();
}

MainWindow::~MainWindow()
{
    Settings::instance()->setWindowGeometry(geometry());
    Settings::instance()->setSplitterSizes(m_splitter->sizes());
    Settings::instance()->setFont(qApp->font());
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

    if ((d_items.isEmpty() || d_items.size() > 1)) {
        QMessageBox::information(this, tr("No items selected"), tr("Please select item."));
        return;
    }
    if (m_controllersEdit->controllerWidget()->row(d_items.first()) == 0) {
        return;
    }
    QSharedPointer<Telnet> controller = m_controllerOwner.controller(d_items.first()->toolTip());
    EditControllerDialog dialog(controller);
    dialog.exec();
}

void MainWindow::execSettingsDialog()
{
    SettingsDialog dialog;
    connect(&dialog, &SettingsDialog::localeChanged, this, &MainWindow::onLanguageChanged);
    connect(&dialog, &SettingsDialog::periodChanged, m_interrogator.data(), &AlarmInterrogator::onPeriodChanged);
    connect(&dialog, &SettingsDialog::autoRefreshChanged, m_interrogator.data(), &AlarmInterrogator::onAutoRefreshChanged);
    dialog.exec();
}

void MainWindow::createSplitter()
{
    m_splitter->addWidget(m_controllersEdit);
    m_splitter->addWidget(m_alarmDisplayWidget);
    ui->horizontalLayout->addWidget(m_splitter.data());

    QList<int> sizes = Settings::instance()->getSplitterSizes();
    if(!sizes.isEmpty()) {
        m_splitter->setSizes(sizes);
    }
}

void MainWindow::aboutProgram()
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Information);
    box.setWindowTitle(tr("About program "));
    box.setText(tr("AlarmBox is a tool designed to simplify the monitoring of Ericsson objects in a 2G network.\nVersion ")
                + qApp->applicationVersion() + tr("\nLastUpdates: ")
               );
    box.setInformativeText(Settings::instance()->getLastUpdates());
    box.exec();
}

void MainWindow::onLanguageChanged(const QLocale &locale)
{
    Q_UNUSED(locale);
    QMessageBox::information(this, tr("Language changed"),
                             tr("The language change will take effect after the next launch."));
}
