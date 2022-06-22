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
    QFont font;
    QLocale language;
    bool refresh;
    bool dvdVideo;
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
    void themeChanged(int themeIndex);
    void fontChanged(const QFont &font);
    void DVDVideoStateChanged(bool state);
private:
    void applySettings();
    void chooseFont();
private:
    Ui::SettingsDialog *m_ui;
    QFont m_font;
    Settings& m_settings;
    InitialValues m_initValues;
};

#endif // SETTINGSDIALOG_H
