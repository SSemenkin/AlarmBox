#ifndef ALARMDISPLAYWIDGET_H
#define ALARMDISPLAYWIDGET_H

#include <QWidget>

class Alarm;

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

signals:
    void refreshRequested();
private:
    Ui::AlarmDisplayWidget *ui;
};

#endif // ALARMDISPLAYWIDGET_H
