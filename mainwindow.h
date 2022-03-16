#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "controllerownership.h"
#include "addcontrollerdialog.h"
#include "editcontrollerdialog.h"
#include "controllersedit.h"
#include "alarminterrogator.h"

class AlarmDisplayWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onLanguageChanged(const QLocale& locale);

private:
    void execAddControllerDialog();
    void execEditControllerDialog();
    void execSettingsDialog();

private:
    Ui::MainWindow *ui;
    ControllerOwnership m_controllerOwner;
    QScopedPointer<AlarmInterrogator> m_interrogator;

    ControllersEdit *m_controllersEdit;
    AlarmDisplayWidget *m_alarmDisplayWidget;
};
#endif // MAINWINDOW_H
