#ifndef ALARMDISPLAYWIDGET_H
#define ALARMDISPLAYWIDGET_H

#include <QWidget>

class Alarm;
class AlarmTreeWidget;

namespace Ui {
class AlarmDisplayWidget;
}

class AlarmDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmDisplayWidget(QWidget *parent = nullptr);
    ~AlarmDisplayWidget();

    void processAlarms(const QVector<Alarm> &alarms);
    void onControllerChanged(const QString &hostname);

    const AlarmTreeWidget *alarmTreeWidget() const;

signals:
    void refreshRequested();
private:
    Ui::AlarmDisplayWidget *ui;
};

#endif // ALARMDISPLAYWIDGET_H
