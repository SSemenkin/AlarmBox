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
    explicit InheritanceView(QWidget *parent = nullptr);
    ~InheritanceView();

    void setObjectsHierarchy(const QHash<Telnet*, QMap<QString, QString>> &objectsHierarchy);
    InheritanceTreeWidget *inheritanceTreeWidget() const;

private:
    Ui::InheritanceView *m_ui;
};

#endif // INHERITANCEVIEW_H
