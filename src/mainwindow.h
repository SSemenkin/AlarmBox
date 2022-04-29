#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "controllerownership.h"
#include "addcontrollerdialog.h"
#include "editcontrollerdialog.h"
#include "controllersedit.h"
#include "alarminterrogator.h"
#include "updatechecker.h"
#include "maps/mapwidget.h"

class AlarmDisplayWidget;
class QSplitter;
class QLabel;
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
    void onThemeChanged(int themeIndex);
    void onFontChanged(const QFont &font);

    void execAddControllerDialog();
    void execEditControllerDialog();
    void execSettingsDialog();
    void createSplitter();
    void aboutProgram();
    void updateButtonClicked();

    QPalette generateDarkPalette() const;
    void restartApplication();
private:
    Ui::MainWindow *m_ui;
    ControllerOwnership m_controllerOwner;
    QScopedPointer<AlarmInterrogator> m_interrogator;
    QScopedPointer<QSplitter> m_splitter;
    QScopedPointer<MapWidget> m_mapWidget;

    ControllersEdit *m_controllersEdit;
    AlarmDisplayWidget *m_alarmDisplayWidget;
    InheritanceView *m_inheritanceView;

    UpdateChecker m_updater;

    QPalette m_lightPalette;
    QPalette m_darkPalette;
};
#endif // MAINWINDOW_H
