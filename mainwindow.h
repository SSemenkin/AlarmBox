#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "controllerownership.h"
#include "addcontrollerdialog.h"
#include "editcontrollerdialog.h"
#include "controllersedit.h"

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
    void execAddControllerDialog();
    void execEditControllerDialog();

private:
    Ui::MainWindow *ui;
    ControllerOwnership m_controllerOwner;
    ControllersEdit *controllersEdit;
};
#endif // MAINWINDOW_H
