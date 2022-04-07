#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "controllerownership.h"
#include "addcontrollerdialog.h"
#include "editcontrollerdialog.h"
#include "controllersedit.h"
#include "alarminterrogator.h"

class AlarmDisplayWidget;
class QSplitter;
class InheritanceView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void onLanguageChanged(const QLocale& locale);
    void onThemeChanged();

    void execAddControllerDialog();
    void execEditControllerDialog();
    void execSettingsDialog();
    void createSplitter();
    void aboutProgram();
    QPalette generateDarkPalette() const;

    void restartApplication();
private:
    Ui::MainWindow *ui;
    ControllerOwnership m_controllerOwner;
    QScopedPointer<AlarmInterrogator> m_interrogator;
    QScopedPointer<QSplitter> m_splitter;

    ControllersEdit *m_controllersEdit;
    AlarmDisplayWidget *m_alarmDisplayWidget;
    InheritanceView *m_inheritanceView;
};
#endif // MAINWINDOW_H
