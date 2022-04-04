#ifndef INHERITANCEVIEW_H
#define INHERITANCEVIEW_H

#include <QWidget>

class Telnet;
class InheritanceTreeWidget;

namespace Ui {
class InheritanceView;
}

class InheritanceView : public QWidget
{
    Q_OBJECT

public:
    explicit InheritanceView(const QHash<Telnet*,  QMap<QString, QString>> &objectsHierarchy
                           , QWidget *parent = nullptr);
    ~InheritanceView();

    InheritanceTreeWidget *inheritanceTreeWidget() const;

private:
    Ui::InheritanceView *ui;
};

#endif // INHERITANCEVIEW_H
