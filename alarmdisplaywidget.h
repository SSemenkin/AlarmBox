#ifndef ALARMDISPLAYWIDGET_H
#define ALARMDISPLAYWIDGET_H

#include <QWidget>

namespace Ui {
class AlarmDisplayWidget;
}

class AlarmDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmDisplayWidget(QWidget *parent = nullptr);
    ~AlarmDisplayWidget();

private:
    Ui::AlarmDisplayWidget *ui;
};

#endif // ALARMDISPLAYWIDGET_H
