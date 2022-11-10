#include "databaseupdaterdialog.h"
#include "ui_databaseupdaterdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include "databaseupdater.h"

DatabaseUpdaterDialog::DatabaseUpdaterDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DatabaseUpdaterDialog)
{
    m_ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    setVisibleProgressWidgets(false);

    connect(m_ui->browse_button, &QPushButton::clicked, this, [this] () -> void {
       m_ui->lineEdit->setText(QFileDialog::getOpenFileName(this, tr("Select source file"), QDir::homePath(), "*.csv"));
    });
    connect(m_ui->requirements, &QPushButton::clicked, this, [this]() -> void {
       QMessageBox::information(this, tr("Requirements"), tr("First row will ignored.\nYou must select file that contains 5 columns (at least).\n"
                                                             "FORMAT {RBS_NAME;CELL_NAME;AZIMUTH;LONGITUDE;LATITUDE.}"));
    });
    connect(m_ui->updateButton, &QPushButton::clicked, this, &DatabaseUpdaterDialog::updateObjectsDatabase);
}

DatabaseUpdaterDialog::~DatabaseUpdaterDialog()
{
    delete m_ui;
}

void DatabaseUpdaterDialog::updateObjectsDatabase()
{
    QString filepath = m_ui->lineEdit->text();
    if (filepath.isEmpty()) {
        QMessageBox::information(this, tr("Error"), tr("Filepath is empty."));
        return;
    }

    if (!m_updater.isNull() && m_updater->isBusy()) {
        QMessageBox::information(this, tr("Error"), tr("Update is running."));
        return;
    }

    m_updater.reset(new DatabaseUpdater(m_ui->lineEdit->text()));
    setVisibleProgressWidgets();
    connect(m_updater.data(), &DatabaseUpdater::progress, m_ui->progressBar, &QProgressBar::setValue);
    connect(m_updater.data(), &DatabaseUpdater::error, m_ui->errors, &QTextBrowser::append);
    connect(m_updater.data(), &DatabaseUpdater::finished, this, [this]() {
        setVisibleProgressWidgets(false);
        m_ui->progressBar->setValue(100);
        QMessageBox::information(this, tr("Info"), tr("Update complete.Change will take effect after the next launch."));
    });
}

void DatabaseUpdaterDialog::setVisibleProgressWidgets(bool state)
{
    if (m_ui->errors->toPlainText().isEmpty()) {
        m_ui->errors->setVisible(state);
    }
    m_ui->progressBar->setVisible(state);
}
