#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLocale>
class Settings;
namespace Ui {
class SettingsDialog;
}

struct InitialValues {
    QLocale language;
    bool refresh;
    uint32_t period;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void localeChanged(const QLocale& locale);
    void periodChanged(uint32_t period);
    void autoRefreshChanged(bool state);
private:
    void applySettings();
private:
    Ui::SettingsDialog *ui;
    Settings& m_settings;
    InitialValues m_initValues;
};

#endif // SETTINGSDIALOG_H
