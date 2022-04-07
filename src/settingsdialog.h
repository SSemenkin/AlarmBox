#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLocale>

class QSimpleUpdater;

class Settings;
namespace Ui {
class SettingsDialog;
}

struct InitialValues {
    QLocale language;
    bool refresh;
    uint32_t period;
    int themeIndex;
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
    void themeChanged();
private:
    void applySettings();
    void chooseFont() const;
    void setupUpdater();
private:
    Ui::SettingsDialog *ui;
    Settings& m_settings;
    InitialValues m_initValues;
    QSimpleUpdater *m_updater;
};

#endif // SETTINGSDIALOG_H
