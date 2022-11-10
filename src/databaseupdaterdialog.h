#ifndef DATABASEUPDATERDIALOG_H
#define DATABASEUPDATERDIALOG_H

#include <QDialog>

class DatabaseUpdater;

namespace Ui {
class DatabaseUpdaterDialog;
}

class DatabaseUpdaterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseUpdaterDialog(QWidget *parent = nullptr);
    ~DatabaseUpdaterDialog();

private:
    void updateObjectsDatabase();
    void setVisibleProgressWidgets(bool state = true);
private:
    Ui::DatabaseUpdaterDialog *m_ui;
    QScopedPointer<DatabaseUpdater> m_updater;
};

#endif // DATABASEUPDATERDIALOG_H
